#ifndef SRC_MAIL_CLASS_H_
#define SRC_MAIL_CLASS_H_

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "tools.h"


class MailForSearch
{
private:
public:
    std::string from, to;
    long long date;
    int id;
    std::unordered_set<std::string> contents;
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
    int length, id;

    MailLength(const int& i, const int& l) : id(i), length(l) {}

    inline int getID() const { return id; }
    inline int getLen() const { return length; }
    bool operator<(const MailLength& y) const
    {
        if (length == y.getLen())
            return id < y.getID();
        return length < y.getLen();
    }
};

class MailSearcher
{
public:
    std::map<int, MailForSearch> mails;

    std::set<MailLength> mail_lens;
    // std::unordered_map<std::string, int> mail_by_from;
    // std::unordered_map<std::string, int> mail_by_to;
    std::multimap<long long, int> mail_by_date;

    std::unordered_map<std::string, bool> query_cache;

    const size_t query_cache_size = 100000;

    void add(const char* file_path);
    void longest() const;
    void query(const char querystr[]) const;
    void remove(int id);
};

#endif
