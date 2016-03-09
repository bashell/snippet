#ifndef _POLL_T_H_
#define _POLL_T_H_

#include <poll.h>

#define CLIENTSIZE 2048

#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

typedef struct {
    struct pollfd clients_[CLIENTSIZE];
    int listenfd_;
    int maxi_;  // clients_的最大下标
    int nready_;
    void (*handle_callback_)(int, char*);  // 回调函数
} poll_t;


void poll_init(poll_t *pol, int listenfd, void (*handler)(int, char*));
void poll_do_wait(poll_t *pol);
void poll_handle_accept(poll_t *pol);
void poll_handle_data(poll_t *pol);


#endif  /* _POLL_T_H_ */
