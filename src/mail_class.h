#ifndef SRC_MAIL_CLASS_H_
#define SRC_MAIL_CLASS_H_

#include <unordered_set>
#include <string>

namespace Roger {

class MailForSearch {
private: 
    std::string from, to;
    long long date;
    int id;
    std::unordered_set<std::string> contents;
    int length;
public:
    MailForSearch(const std::string& f, const std::string& t, const long long& d, const int& i)
        : from(f), to(t), date(d), id(i) {};
    inline long long getDate() const{return date;}
    inline int getID() const{return id;}
    inline int getLen() const{return length;}
public:
    inline void insertContent(const std::string& str) {contents.insert(str);}
    inline bool isFrom(const std::string& str) {return str == from;}
    inline bool isTo(const std::string& str) {return str == to;}
    inline void setLength(const int& l) {length = l;}
    bool queryString(const std::string& str);
    void print() const;
    bool operator<(const MailForSearch& y) const{
        return id < y.getID();
    }
};

class MailLength {
private:
    int length, id;
public:
    MailLength(const int& i, const int& l) : id(i), length(l) {}
    
    inline int getID() const{return id;}
    inline int getLen() const{return length;}
    bool operator<(const MailLength& y) const{
        if(length == y.getLen()) return id < y.getID();
        return length < y.getLen(); 
    }
};


} //namespace of XuMingSheng

#endif
