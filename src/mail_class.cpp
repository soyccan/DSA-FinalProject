#include "mail_class.h"
#include <cassert>
#include <cctype>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>
#include "query.h"
#include "search_mail_functions.h"
#include "tools.h"

#define MAX_READ_SIZE 1000000

typedef std::vector<std::string> Expression;

struct QueryOpt {
    std::string from, to;
    // std::tm date_from, date_to;
    long long date_from, date_to;
    bool has_date_from, has_date_to;
    QueryOpt()
        : date_from(0), date_to(0), has_date_from(false), has_date_to(false)
    {
    }
};

bool MailForSearch::queryString(const std::string& str) const
{
    auto result = contents.find(str);
    if (result == contents.end())
        return false;
    return true;
}
void MailForSearch::print() const
{
    std::cout << from << "\n" << date << "\n" << id << "\n" << to << "\n";
    std::cout << "contents size: " << contents.size() << "\n";
    return;
}

static inline long long cutDate()
{
    char* token = strtok(NULL, "\n ");
    int day = atoi(strtok(NULL, "\n "));
    char* monthName = strtok(NULL, "\n ");

    char months[12][10] = {"January",   "February", "March",    "April",
                           "May",       "June",     "July",     "August",
                           "September", "October",  "November", "December"};
    int month = 0;
    for (int i = 0; i < 12; i++) {
        if (strcmp(monthName, months[i]) == 0) {
            month = i + 1;
            break;
        }
    }

    int year = atoi(strtok(NULL, "\n "));
    token = strtok(NULL, "\n ");
    int hour = atoi(strtok(NULL, "\n :"));
    int minute = atoi(strtok(NULL, "\n :"));
    // LOG("cutdate %d/%d/%d %d:%d",year,month,day,hour,minute);

    long long date = 0;
    date += year, date *= 100;
    date += month, date *= 100;
    date += day, date *= 100;
    date += hour, date *= 100;
    date += minute;
    return date;
}

void MailSearcher::add(const char* file_path)
{
    FILE* file = fopen(file_path, "r");
    assert(file != NULL);
    // std::vector<char> words(MAX_READ_SIZE);
    char words[MAX_READ_SIZE];
    int read_size = fread(&words[0], sizeof(char), MAX_READ_SIZE, file);
    words[read_size] = '\0';

    char* token = strtok(&words[0], "\n ");
    char* from = strtok(NULL, "\n ");
    for (char* c = from; *c; c++)
        // from is case-insensitive
        *c = std::tolower(*c);

    long long date = cutDate();

    token = strtok(NULL, "\n ");
    int id = atoi(strtok(NULL, "\n "));
    if (mails.find(id) != mails.end()) {
        OUT("-\n");
        fclose(file);
        return;
    }

    token = strtok(NULL, "\n ");
    for (char* i = token; *i != '\n'; i++) {
        if (isdigit(*i) == 0 && isalpha(*i) == 0)
            *i = ' ';
    }
    char* content = strtok(NULL, "\n ");
    int len = strlen(content);
    std::vector<char*> subject;
    while (len < 3 || strcmp(content + len - 4, "\nTo:") != 0) {
        subject.push_back(content);
        content = strtok(NULL, "\n ");
        len = strlen(content);
    }

    char* to = strtok(NULL, "\n ");
    for (char* c = to; *c; c++)
        // to is case-insensitive
        *c = std::tolower(*c);

    MailForSearch mail =
        MailForSearch(std::string(from), std::string(to), date, id);
    for (int i = 0; i < subject.size(); i++) {
        mail.insertContent(std::string(subject[i]));
    }

    token = strtok(NULL, "\n ");
    for (char* i = token; i != words + read_size; i++) {
        if (isdigit(*i) == 0 && isalpha(*i) == 0)
            *i = ' ';
    }
    content = strtok(NULL, "\n ");
    len = 0;
    while (content != NULL) {
        len += strlen(content);
        mail.insertContent(content);
        content = strtok(NULL, "\n ");
    }
    mail.setLength(len);
    // mail.print();
    LOG("new mail id=%d date=%lld", mail.getID(), mail.getDate());
    mails[mail.id] = mail;
    mail_lens.insert(MailLength(id, len));

    mail_by_from[mail.from].push_back(mail.id);
    mail_by_to[mail.to].push_back(mail.id);
    // mail_by_date.insert(std::make_pair(mail.date, mail.id));

    OUT("%lu\n", mails.size());
    fclose(file);
    return;
}

void MailSearcher::remove(int id)
{
    auto it = mails.find(id);
    if (it == mails.end()) {
        OUT("-\n");
        return;
    }
    auto& mail = it->second;
    int len = mail.getLen();
    mails.erase(it);
    mail_lens.erase(MailLength(id, len));
    OUT("%lu\n", mails.size());

    // LOG("want remove date=%lld id=%d", mail.date, mail.id);

    // auto st = mail_by_date.lower_bound(mail.date);
    // auto en = mail_by_date.upper_bound(mail.date);
    // assert(st != en);
    // assert(st != mail_by_date.end());
    // // LOG("first date=%lld id=%d", st->first, st->second);
    // // LOG("second date=%lld id=%d", en->first, en->second);
    // for (auto it = st; it != en; it++) {
    //     // LOG("remove date=%lld id=%d", it->first, it->second);
    //     if (it->second == mail.id) {
    //         mail_by_date.erase(it);
    //         break;
    //     }
    // }
    {
        auto& target = mail_by_from[mail.from];
        for (auto it = target.begin(); it != target.end(); it++) {
            if (*it == mail.id) {
                target.erase(it);
                break;
            }
        }
    }
    {
        auto& target = mail_by_to[mail.to];
        for (auto it = target.begin(); it != target.end(); it++) {
            if (*it == mail.id) {
                target.erase(it);
                break;
            }
        }
    }

    return;
}

void MailSearcher::longest() const
{
    if (mail_lens.size() == 0) {
        OUT("-\n");
        return;
    }
    auto it = mail_lens.rbegin();
    OUT("%d %d\n", it->getID(), it->getLen());
    return;
}

static inline bool _is_prior(const std::string& a, const std::string& b)
{
    assert(a != "(");
    assert(a != ")");
    assert(b != "(");
    assert(b != ")");

    int p1 = 0, p2 = 0;
    if (a == "!")
        p1 = 1;
    else if (a == "&")
        p1 = 2;
    else if (a == "|")
        p1 = 3;
    else
        assert(false);

    if (b == "!")
        p2 = 1;
    else if (b == "&")
        p2 = 2;
    else if (b == "|")
        p2 = 3;
    else
        assert(false);

    if (p1 != p2) {
        return p1 < p2;
    } else {
        return (a != "!" && b != "!") || !(a == "!" && b == "!");
    }
}

static inline Expression _infix_to_postix(const Expression& exprlist)
{
    // LOG("_infix_to_postix");
    std::vector<std::string> postfix;
    std::vector<std::string> tmp;

#define __stack_status(action)      \
    {                               \
        LOGN(#action " postfix:");  \
        for (auto& x : postfix) {   \
            LOGN("%s ", x.c_str()); \
        }                           \
        LOGN("\ttmp:");             \
        for (auto& x : tmp) {       \
            LOGN("%s ", x.c_str()); \
        }                           \
        LOG("");                    \
    }
#undef __stack_status
#define __stack_status(action)

    for (auto& op : exprlist) {
        // LOG("current op %s", op.c_str());

        if (!isalnum(op[0])) {  // operator
            if (op == "(") {
                tmp.push_back(op);
                __stack_status(push);
            } else if (op == ")") {
                while (!tmp.empty() && tmp.back() != "(") {
                    postfix.push_back(tmp.back());
                    tmp.pop_back();
                    __stack_status(move);
                }
                assert(tmp.size() > 0);
                tmp.pop_back();
                __stack_status(pop);
            } else {
                while (!tmp.empty() && tmp.back() != "(" &&
                       _is_prior(tmp.back(), op)) {
                    postfix.push_back(tmp.back());
                    tmp.pop_back();
                    __stack_status(move);
                }
                tmp.push_back(op);
                __stack_status(push);
            }
        } else {  // keyword
            assert(op != "");
            postfix.push_back(op);
            __stack_status(push);
        }
    }
    while (!tmp.empty()) {
        postfix.push_back(tmp.back());
        tmp.pop_back();
        __stack_status(move);
    }
#undef __stack_status

    assert(tmp.empty());
    // for (auto& op : postfix)
    //     LOG("postfix: %s", op.c_str());
    return postfix;
}

static inline Expression _parse_expr(const char expression[])
{
    // LOG("_parse_expr");
    Expression exprlist;
    std::string buf;
    for (int i = 0; expression[i] != '\0'; i++) {
        if (isalnum(expression[i])) {
            buf.push_back(
                std::tolower(expression[i]));  // keyword are case-insensitive
        } else {
            assert(expression[i] == '&' || expression[i] == '|' ||
                   expression[i] == '!' || expression[i] == '(' ||
                   expression[i] == ')');
            if (!buf.empty()) {
                exprlist.push_back(buf);
                buf.clear();
            }
            exprlist.emplace_back(1, expression[i]);
        }
    }
    if (!buf.empty()) {
        exprlist.push_back(buf);
    }
    // for (auto& op : exprlist) {
    //     LOG("parse_expr op %s",op.c_str());
    // }
    return exprlist;
}

static inline void _parse_qs(const char querystr[],
                             Expression& exprlist,
                             QueryOpt& queryopt)
{
    // LOG("_parse_qs");
    char* qs = strdup(querystr);
    for (char* c = qs; *c; c++)
        // case-insensitive
        *c = std::tolower(*c);
    for (char* token = strsep(&qs, " "); token != NULL;
         token = strsep(&qs, " ")) {
        // LOG("token %s",token);
        if (token[0] == '-') {
            if (token[1] == 'f') {
                assert(token[2] == '"');
                token = token + 3;
                *strchr(token, '"') = '\0';
                queryopt.from = std::string(token);
                LOG("qs from %s", token);
            } else if (token[1] == 't') {
                assert(token[2] == '"');
                token = token + 3;
                *strchr(token, '"') = '\0';
                queryopt.to = std::string(token);
                LOG("qs to %s", token);
            } else if (token[1] == 'd') {
                char* sto = token + 2;
                char* sfrom = strsep(&sto, "~");
                if (*sfrom) {
                    queryopt.has_date_from = true;
                    queryopt.date_from = std::atoll(sfrom);
                    LOG("qs date from %lld", queryopt.date_from);

                    // queryopt.date_from.tm_min = std::atoi(sfrom + 10);
                    // sfrom[10] = '\0';
                    // queryopt.date_from.tm_hour = std::atoi(sfrom + 8);
                    // sfrom[8] = '\0';
                    // queryopt.date_from.tm_mday = std::atoi(sfrom + 6);
                    // sfrom[6] = '\0';
                    // queryopt.date_from.tm_mon = std::atoi(sfrom + 4);
                    // sfrom[4] = '\0';
                    // queryopt.date_from.tm_year = std::atoi(sfrom);
                    //
                    // LOG("qs date from %d/%d/%d %d:%d",
                    //     queryopt.date_from.tm_year,
                    //     queryopt.date_from.tm_mon,
                    //     queryopt.date_from.tm_mday,
                    //     queryopt.date_from.tm_hour,
                    //     queryopt.date_from.tm_min);
                }
                if (*sto) {
                    queryopt.has_date_to = true;
                    queryopt.date_to = std::atoll(sto);
                    LOG("qs date to %lld", queryopt.date_to);

                    // queryopt.date_to.tm_min = std::atoi(sto + 10);
                    // sto[10] = '\0';
                    // queryopt.date_to.tm_hour = std::atoi(sto + 8);
                    // sto[8] = '\0';
                    // queryopt.date_to.tm_mday = std::atoi(sto + 6);
                    // sto[6] = '\0';
                    // queryopt.date_to.tm_mon = std::atoi(sto + 4);
                    // sto[4] = '\0';
                    // queryopt.date_to.tm_year = std::atoi(sto);
                    //
                    // LOG("qs date to %d/%d/%d %d:%d",
                    // queryopt.date_to.tm_year,
                    //     queryopt.date_to.tm_mon, queryopt.date_to.tm_mday,
                    //     queryopt.date_to.tm_hour, queryopt.date_to.tm_min);
                }
            } else {
                assert(false);
            }
        } else {  // expression
            // LOG("expression %s",token);
            exprlist = _parse_expr(token);
        }
    }
    free(qs);
    qs = NULL;
}

static inline bool _test_expr(
    const MailForSearch& mail,
    const Expression& postfix_expr,
    std::unordered_map<std::string, std::vector<bool>>& query_cache)
{
    std::vector<bool> st;  // stack

    for (auto& op : postfix_expr) {
        if (!isalnum(op[0])) {  // operator
            if (op == "!") {
                assert(st.size() >= 1);
                st.back() = !st.back();
            } else {
                assert(op == "|" || op == "&");
                assert(st.size() >= 2);
                bool x = st.back();
                st.pop_back();
                st.back() = op == "|" ? st.back() | x : st.back() & x;
            }
        } else {  // keyword
            int haskw = mail.queryString(op);
            st.push_back(haskw);
            // if (haskw)
            //     query_cache[op].push_back(mail.id);
        }
    }

    assert(st.size() == 1);
    return st.back();
}

void MailSearcher::query(const char querystr[])
{
    Expression exprlist;
    QueryOpt queryopt;
    _parse_qs(querystr, exprlist, queryopt);
    exprlist = _infix_to_postix(exprlist);
    std::vector<int> res;

    bool has_from = queryopt.from != "";
    bool has_to = queryopt.to != "";

    if (has_from || has_to) {
        std::vector<int> subset;  // store from=from && to=to
        if (has_from && has_to) {
            std::vector<int> sub1, sub2;  // store from=from && to=to
            for (int id : mail_by_from[queryopt.from]) {
                sub1.push_back(id);
            }
            for (int id : mail_by_to[queryopt.to]) {
                sub2.push_back(id);
            }
            std::sort(sub1.begin(), sub1.end());
            std::sort(sub2.begin(), sub2.end());
            std::set_intersection(sub1.begin(), sub1.end(), sub2.begin(),
                                  sub2.end(), std::back_inserter(subset));
        } else if (has_from) {
            for (int id : mail_by_from[queryopt.from]) {
                subset.push_back(id);
            }
        } else {
            assert(has_to);
            for (int id : mail_by_to[queryopt.to]) {
                subset.push_back(id);
            }
        }
        for (int id : subset) {
            auto it = mails.find(id);
            assert(it != mails.end());
            auto& mail = it->second;
            if ((!queryopt.has_date_from || mail.date >= queryopt.date_from) &&
                (!queryopt.has_date_to || mail.date <= queryopt.date_to)) {
            } else {
                continue;
            }
            LOG("subset in date range id=%d date=%lld", mail.id, mail.date);

            if (_test_expr(mail, exprlist, query_cache)) {
                res.push_back(mail.id);
            }
        }
        if (!res.empty()) {
            std::sort(res.begin(), res.end());
            for (size_t i = 0; i < res.size(); i++) {
                OUT("%s%d", i == 0 ? "" : " ", res[i]);
            }
            OUT("\n");
        } else {
            OUT("-\n");
        }
        return;
    } else {
        // auto st = queryopt.has_date_from
        //               ? mail_by_date.lower_bound(queryopt.date_from)
        //               : mail_by_date.begin();
        // auto en = queryopt.has_date_to ?
        // mail_by_date.upper_bound(queryopt.date_to)
        //                                : mail_by_date.end();
        // for (auto it = st; it != en; it++) {
        //     // LOG("date=%lld id=%d", it->first, it->second);
        //     int i = it->second;
        //
        //     auto mailit = mails.find(i);
        //     assert(mailit != mails.end());
        //     auto& mail = mailit->second;
        for (const auto& [id, mail] : mails) {
            // for (auto& x : mail.contents) {
            //     LOG("  content %s",x.c_str());
            // }

            if ((!queryopt.has_date_from || mail.date >= queryopt.date_from) &&
                (!queryopt.has_date_to || mail.date <= queryopt.date_to)) {
            } else {
                continue;
            }
            // LOG("mail in date range id=%d date=%lld", mail.id, mail.date);

            if (_test_expr(mail, exprlist, query_cache)) {
                res.push_back(mail.id);
            }
        }
        if (!res.empty()) {
            // std::sort(res.begin(), res.end());
            for (size_t i = 0; i < res.size(); i++) {
                OUT("%s%d", i == 0 ? "" : " ", res[i]);
            }
            OUT("\n");
        } else {
            OUT("-\n");
        }
        return;
    }
}
