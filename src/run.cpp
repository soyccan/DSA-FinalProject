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
            int x = roger.add(file_path);
            if (x == -1) {
                OUT("-\n");
            } else {
                OUT("%d\n", x);
            }
        } else if (strcmp(query, "remove") == 0) {
            int id;
            scanf("%d", &id);
            LOGCLR("%d remove %d", cnt, id);
            int x = roger.remove(id);
            if (x == -1) {
                OUT("-\n");
            } else {
                OUT("%d\n", x);
            }
        } else if (strcmp(query, "longest") == 0) {
            LOGCLR("%d longest", cnt);
            int id, len;
            std::tie(id, len) = roger.longest();
            if (id == -1) {
                OUT("-\n");
            } else {
                OUT("%d %d\n", id, len);
            }
        } else if (strcmp(query, "query") == 0) {
            scanf(" %[^\n]\n", querystr);
            LOGCLR("%d query %s", cnt, querystr);
            std::vector<int> v = roger.query(querystr);
            if (v.empty()) {
                OUT("-\n");
            } else {
                for (size_t i = 0; i < v.size(); i++) {
                    OUT("%s%d\n", i == 0 ? "" : " ", v[i]);
                }
            }
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
