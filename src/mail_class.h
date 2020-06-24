#ifndef SRC_MAIL_CLASS_H_
#define SRC_MAIL_CLASS_H_

#include <algorithm>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

#include "MurmurHash2.cpp"
#include "MurmurHash3.cpp"
#include "city.h"
#include "tools.h"

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


struct murmurhash : std::hash<std::string> {
    std::size_t operator()(const std::string& s) const
    {
        // size_t res[2];
        // MurmurHash3_x64_128(s.c_str(), s.size(), 0x9453, &res);
        // return MurmurHash64A(s.c_str(), s.size(), 94389);
        // return (size_t) res[0] ^ res[1];
        // return CityHash64(s.c_str(), s.size());

        const char* _s = s.c_str();

        // djb2
        unsigned long hash = 5381;
        int c;
        while (c = *_s++)
            hash = ((hash << 5) + hash) + c;  // hash * 33 + c
        return hash;
    }
};

namespace std
{
template <>
struct __is_fast_hash<murmurhash> : std::false_type {
};
};  // namespace std

struct pairhash {
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& x) const
    {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};


class MailForSearch
{
private:
public:
    std::string from, to;
    long long date;
    int id;
    __gnu_pbds::gp_hash_table<std::string, __gnu_pbds::null_type, murmurhash>
        contents;
    int length;

    MailForSearch() {}
    MailForSearch(const std::string& f,
                  const std::string& t,
                  const long long& d,
                  const int& i)
        : from(f), to(t), date(d), id(i), length(0){};
    inline long long getDate() const { return date; }
    inline int getID() const { return id; }
    inline int getLen() const { return length; }

    inline void insertContent(const std::string& str)
    {
        // Note: mail subject/content is case-insensitive
        std::string s = str;
        for (size_t i = 0; i < s.size(); i++)
            s[i] = std::tolower(s[i]);
        contents.insert(s);
    }
    inline bool isFrom(const std::string& str) const { return str == from; }
    inline bool isTo(const std::string& str) const { return str == to; }
    void setLength(const int& l)
    {
        length = l;
    }  // TODO inlining this function causes weird behaviour
    bool queryString(const std::string& str) const;
    void print() const;
    bool operator<(const MailForSearch& y) const { return id < y.getID(); }
};

class MailLength
{
private:
public:
    int length;
    const MailForSearch* mail;

    MailLength(const MailForSearch* mail, const int& l) : mail(mail), length(l)
    {
    }

    inline int getLen() const { return length; }
    bool operator<(const MailLength& y) const
    {
        if (length == y.length)
            return mail->id > y.mail->id;
        return length < y.length;
    }
};

class MailSearcher
{
public:
    __gnu_pbds::tree<int, MailForSearch*> mails;       // id -> mail
    __gnu_pbds::tree<int, MailForSearch*> recyclebin;  // id -> mail
    __gnu_pbds::tree<MailLength, __gnu_pbds::null_type>
        mail_lens;  // length -> mail

    __gnu_pbds::
        gp_hash_table<std::string, std::vector<MailForSearch*>, murmurhash>
            mail_by_from;
    __gnu_pbds::
        gp_hash_table<std::string, std::vector<MailForSearch*>, murmurhash>
            mail_by_to;
    // std::multimap<long long, int> mail_by_date;

    // (mail id, keyword) -> bool
    // std::map<std::pair<int, std::string>, bool> query_cache;

    // const size_t query_cache_size = 1000;

    int add(const char* file_path);
    std::pair<int, int> longest() const;
    int remove(int id);

    std::vector<int> query(const char querystr[]);

private:
    inline bool _test_expr(const MailForSearch* mail,
                           const Expression& postfix_expr);
};

#endif
