#include "mail_class.h"
#include <iostream>

namespace Roger {

bool MailForSearch::queryString(const std::string& str) const {
    auto result = contents.find(str);
    if (result == contents.end()) return false;
    return true;
}
void MailForSearch::print() const{
    std::cout << from << "\n" << date << "\n" << id << "\n" << to <<"\n";
    std::cout << "contents size: " << contents.size() << "\n";
    return;
}

} //namespace of XuMingSheng

