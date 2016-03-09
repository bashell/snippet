#ifndef _SELECT_T_H_
#define _SELECT_T_H_

#include <sys/select.h>


#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)


typedef struct {
    fd_set allset_;            // rset_的备份
    fd_set rset_;
    int clients_[FD_SETSIZE];  // 已连接客户的fd
    int maxi_;                 // clients_的最大下标
    int maxfd_;                // 最大fd
    int nready_;
    int listenfd_;
    void (*handle_callback_)(int, char*);   // 回调函数
} select_t;


void select_init(select_t *sel, int listenfd);
void select_set_callback(select_t *sel, void (*handle_callback)(int, char*));
int select_do_wait(select_t *sel);
void select_handle_accept(select_t *sel);
void select_handle_data(select_t *sel);


#endif  /* _SELECT_T_H_ */
