/* convinient functions */
#ifndef _DSA_TOOLS_H_
#define _DSA_TOOLS_H_ 1

#include <cstdio>

/* debug only */
#ifndef NDEBUG
#define LOG(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#define LOGN(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define LOGCLR(fmt, ...) fprintf(stderr, "\e[31m" fmt "\e[0m\n", ##__VA_ARGS__)
// stdout and stderr at the same time
#define OUT(fmt, ...)                        \
    {                                        \
        fprintf(stderr, fmt, ##__VA_ARGS__); \
        printf(fmt, ##__VA_ARGS__);          \
    }

#else
#define LOG(...)
#define LOGN(...)
#define LOGCLR(...)
#define OUT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif
/* end debug only */


#define FOR(type, i, start, end) for (type i = (start); i < (end); i++)

#endif
