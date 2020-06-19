#ifndef SRC_QUERY_H_
#define SRC_QUERY_H_ 1
#include <set>
#include <vector>

#include "mail_class.h"

using namespace Roger;

namespace query_support
{
void query(const char querystr[], std::set<MailForSearch>& mails);

};  // namespace query_support

#endif
