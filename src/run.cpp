#include <climits>
#include <cstdio>
#include <string>
#include <vector>
#include "mail_class.h"
#include "query.h"
#include "search_mail_functions.h"
#include "tools.h"

int main(void)
{
    char querystr[100000];
    char query[1024];
    char file_path[128];
    // std::set<Roger::MailForSearch> mails;
    // std::set<Roger::MailLength> mail_lens;
    MailSearcher roger;
    int cnt = 0;
    while (scanf("%s", query) != EOF) {
        if (strcmp(query, "add") == 0) {
            scanf("%s", file_path);
            LOGCLR("%d add %s", cnt, file_path);
            roger.add(file_path);
        } else if (strcmp(query, "remove") == 0) {
            int id;
            scanf("%d", &id);
            LOGCLR("%d remove %d", cnt, id);
            roger.remove(id);
        } else if (strcmp(query, "longest") == 0) {
            LOGCLR("%d longest", cnt);
            roger.longest();
        } else if (strcmp(query, "query") == 0) {
            scanf(" %[^\n]\n", querystr);
            LOGCLR("%d query %s", cnt, querystr);
            roger.query(querystr);
        } else {
            LOGCLR("FUCK query=%s", query);
            assert(false);
        }
        cnt++;
        // if (cnt % 1000 == 0) printf("%lu\n", mail_lens.size());
    }
    // printf("%lu\n", mails.size());

    return 0;
}
