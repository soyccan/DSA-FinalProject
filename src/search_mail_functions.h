#ifndef SRC_SEARCH_MAIL_FUNCTIONS_H_
#define SRC_SEARCH_MAIL_FUNCTIONS_H_

#include "mail_class.h"
#include <set>
#include <cassert>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <cstring>

namespace Roger {

void add(const char* file_path, std::set<MailForSearch>& mails, std::set<MailLength>& mail_lens);

void remove(const int& id, std::set<MailForSearch>& mails, std::set<MailLength>& mail_lens); 

void longest(const std::set<MailLength>& mail_lens); 

} //namespace of XuMingSheng
#endif
