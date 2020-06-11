#include "search_mail_functions.h"
#include <cassert>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <cstring>

namespace Roger {

#define MAX_READ_SIZE 1000000 



static long long cutDate() {
    char *token = strtok(NULL, "\n ");
    int day = atoi(strtok(NULL, "\n "));
    char *monthName = strtok(NULL, "\n ");
    
    char months[12][10] = {"January","February", "March", "April",  "May", "June", "July", "August", "September", "October", "November", "December"};
    int month;
    for (int i = 0; i < 12; i++) {
        if (strcmp(monthName, months[i]) == 0) {
            month = i + 1; break;
        } 
    }
    
     int year = atoi(strtok(NULL, "\n "));
     token = strtok(NULL, "\n ");
     int hour = atoi(strtok(NULL, "\n :"));
     int minute = atoi(strtok(NULL, "\n :"));

     long long date = 0;
     date += year, date *= 100;
     date += month, date *= 100;
     date += day, date *= 100;
     date += hour, date *= 100;
     date += minute;
     return date; 
} 
void add(const char* file_path, std::set<MailForSearch>& mails, std::set<MailLength>& mail_lens) {
    FILE* file = fopen(file_path, "r");
    assert(file != NULL);
    //std::vector<char> words(MAX_READ_SIZE);
    char words[MAX_READ_SIZE];
    int read_size = fread(&words[0], sizeof(char), MAX_READ_SIZE, file);
    words[read_size] = '\0';
        
        char *token = strtok(&words[0], "\n ");
        char *from = strtok(NULL, "\n ");

        long long date = cutDate(); 
        
        token = strtok(NULL, "\n ");
        int id = atoi(strtok(NULL, "\n "));
        if(mails.find(MailForSearch("N", "N", 0, id)) != mails.end()) {
            printf("-\n"); fclose(file); return;
        }

        token = strtok(NULL, "\n ");
        for(char* i = token; *i != '\n'; i++) {
            if (isdigit(*i) == 0 && isalpha(*i) == 0)
                *i = ' ';
        }
        char* content = strtok(NULL, "\n ");
        int len = strlen(content);
        std::vector<char*> subject;
        while (len < 3 || strcmp(content + len - 4, "\nTo:") != 0) {
            subject.push_back(content);
            content = strtok(NULL, "\n ");
            len = strlen(content);
        }
        
        char *to = strtok(NULL, "\n ");
        
        
        MailForSearch mail = MailForSearch(std::string(from), std::string(to), date, id);
        for(int i = 0; i < subject.size(); i++) {
            mail.insertContent(std::string(subject[i])); 
        }
       
        token = strtok(NULL, "\n ");
        for(char* i = token; i != words + read_size; i++) {
            if (isdigit(*i) == 0 && isalpha(*i) == 0)
                *i = ' ';
        }
        content = strtok(NULL, "\n ");
        len = 0;
        while (content != NULL) {
            len += strlen(content);
            mail.insertContent(content);
            content = strtok(NULL, "\n ");
        }
        mail.setLength(len);
        //mail.print();
        mails.insert(mail);
        mail_lens.insert(MailLength(id, len));

    printf("%lu\n", mails.size()); 
    fclose(file);
    return; 
}
void remove(const int& id, std::set<MailForSearch>& mails, std::set<MailLength>& mail_lens) {
     MailForSearch tmp = MailForSearch("N", "N", 0, id);
     std::set<MailForSearch>::iterator mail = mails.find(tmp);
     if (mail == mails.end()) {
         printf("-\n"); return;
     }
     int len = mail->getLen();
     mails.erase(tmp);
     mail_lens.erase(MailLength(id, len));
     printf("%lu\n", mails.size()); 
     return;
}

void longest(const std::set<MailLength>& mail_lens){
    if (mail_lens.size() == 0) {
        printf("-\n"); return;
    }
    auto it = mail_lens.rbegin();
    printf("%d %d\n", it->getID(), it->getLen());
    return;

} 

}//namespace of XuMingSheng
