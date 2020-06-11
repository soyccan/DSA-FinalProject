#include "mail_class.h"
#include "search_mail_functions.h"
#include <cstdio>
#include <vector>
#include <string>
#include <climits>

int main(void)
{
    char query[1024];
    char file_path[128];
    std::set<Roger::MailForSearch> mails;
    std::set<Roger::MailLength> mail_lens;
    int cnt = 0;
    while (scanf("%s", query) != EOF) {
        if (strcmp(query, "add") == 0) {
            scanf("%s", file_path);
            Roger::add(file_path, mails, mail_lens);
        }
        else if (strcmp(query, "remove") == 0) {
            int id; scanf("%d", &id);
            Roger::remove(id, mails, mail_lens);
        }
        else if (strcmp(query, "longest") == 0) {
            Roger::longest(mail_lens);
        }
        else if (strcmp(query, "query") == 0) {
            printf("-\n");
        }
        cnt++;
        //if (cnt % 1000 == 0) printf("%lu\n", mail_lens.size());
    }
    //printf("%lu\n", mails.size());

    return 0;
}
