#include "mail_class.h"
#include "search_mail_functions.h"
#include "query.h"
#include "tools.h"
#include <cstdio>
#include <vector>
#include <string>
#include <climits>

int main(void)
{
    char querystr[100000];
    char query[1024];
    char file_path[128];
    std::set<Roger::MailForSearch> mails;
    std::set<Roger::MailLength> mail_lens;
    int cnt = 0;
    while (scanf("%s", query) != EOF) {
        if (strcmp(query, "add") == 0) {
            scanf("%s", file_path);
            LOGCLR("add %s",file_path);
            Roger::add(file_path, mails, mail_lens);
        }
        else if (strcmp(query, "remove") == 0) {
            int id; scanf("%d", &id);
            LOGCLR("remove %d",id);
            Roger::remove(id, mails, mail_lens);
        }
        else if (strcmp(query, "longest") == 0) {
            LOGCLR("longest");
            Roger::longest(mail_lens);
        }
        else if (strcmp(query, "query") == 0) {
            scanf(" %[^\n]\n", querystr);
            LOGCLR("query %s",querystr);
            query_support::query(querystr, mails);
        }
        cnt++;
        //if (cnt % 1000 == 0) printf("%lu\n", mail_lens.size());
    }
    //printf("%lu\n", mails.size());

    return 0;
}
