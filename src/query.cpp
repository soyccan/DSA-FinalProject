#include <cstring>
#include <ctime>
#include <iomanip>
#include <set>
#include <sstream>
#include <vector>
#include <cassert>
#include <cctype>

#include "mail_class.h"
#include "query.h"
#include "tools.h"

using namespace Roger;

namespace query_support
{
typedef std::vector<std::string> Expression;

struct QueryOpt {
    std::string from, to;
    // std::tm date_from, date_to;
    long long date_from, date_to;
    bool has_date_from, has_date_to;
    QueryOpt(): has_date_from(false), has_date_to(false) {}
};

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
    for (auto& op : postfix)
        LOG("postfix: %s", op.c_str());
    return postfix;
}

static inline Expression _parse_expr(const char expression[])
{
    // LOG("_parse_expr");
    const size_t MAX_KEYWORD_SIZE = 20;
    Expression exprlist;
    std::string buf;
    for (int i = 0; expression[i] != '\0'; i++) {
        if (isalnum(expression[i])) {
            buf.push_back(std::tolower(expression[i])); // keyword are case-insensitive
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
                    LOG("qs date from %lld",queryopt.date_from);

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
                    //     queryopt.date_from.tm_year, queryopt.date_from.tm_mon,
                    //     queryopt.date_from.tm_mday, queryopt.date_from.tm_hour,
                    //     queryopt.date_from.tm_min);
                }
                if (*sto) {
                    queryopt.has_date_to = true;
                    queryopt.date_to = std::atoll(sto);
                    LOG("qs date to %lld",queryopt.date_to);

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
                    // LOG("qs date to %d/%d/%d %d:%d", queryopt.date_to.tm_year,
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

static inline bool _test_expr(const MailForSearch& mail, const Expression& postfix_expr) {
    std::vector<bool> st; // stack

    for (auto& op : postfix_expr) {
        if (!isalnum(op[0])) {  // operator
            if (op == "!"){
                assert(st.size() >= 1);
                st.back() = !st.back();
            } else {
                assert(op == "|" || op == "&");
                assert(st.size() >= 2);
                bool x = st.back();
                st.pop_back();
                st.back() = op == "|" ? st.back() | x : st.back() & x;
            }
        }
        else { // keyword
            st.push_back(mail.queryString(op));
        }
    }

    assert(st.size() == 1);
    return st.back();
}

void query(const char querystr[], std::set<MailForSearch>& mails)
{
    Expression exprlist;
    QueryOpt queryopt;
    _parse_qs(querystr, exprlist, queryopt);
    exprlist = _infix_to_postix(exprlist);
    std::vector<int> res;

    for (auto& mail : mails) {
        // for (auto& x : mail.contents) {
        //     LOG("  content %s",x.c_str());
        // }

        if ((!queryopt.has_date_from || mail.getDate() >= queryopt.date_from)
                && (!queryopt.has_date_to || mail.getDate() <= queryopt.date_to)
                && (queryopt.from == "" || mail.isFrom(queryopt.from))
                && (queryopt.to == "" || mail.isTo(queryopt.to))) {
        }
        else {
            continue;
        }
        LOG("mail in date range id=%d date=%lld",mail.getID(),mail.getDate());

        if (_test_expr(mail, exprlist)) {
            res.push_back(mail.getID());
        }
    }
    if (!res.empty()) {
        for (size_t i = 0; i < res.size(); i++) {
            OUT("%s%d", i == 0 ? "" : " ", res[i]);
        }
        OUT("\n");
    }
    else {
        OUT("-\n");
    }
}

};  // namespace query_support
