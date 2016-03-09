#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "select_t.h"
#include "network.h"


void select_add_fd(select_t*, int);
void select_del_fd(select_t*, int);


void select_init(select_t *sel, int listenfd) {
    int i;
    FD_ZERO(&sel->allset_);
    FD_ZERO(&sel->rset_);
    FD_SET(listenfd, &sel->allset_);
    for(i = 0; i < FD_SETSIZE; ++i)
        sel->clients_[i] = -1;
    sel->maxi_ = -1;
    sel->maxfd_ = listenfd;
    sel->nready_ = 0;
    sel->listenfd_ = listenfd;
    sel->handle_callback_ = NULL;
}


void select_set_callback(select_t *sel, void (*handler)(int, char*)) {
    sel->handle_callback_ = handler;
}


int select_do_wait(select_t *sel) {
    int nready;
    sel->rset_ = sel->allset_;  // 获取备份
   
    do {
        nready = select(sel->maxfd_ + 1, &sel->rset_, NULL, NULL, NULL);
    } while(nready == -1 && errno == EINTR);  // interrupt时继续等待
    if(nready == -1)  // 其他ERROR
        ERR_EXIT("select");
    sel->nready_ = nready;
    return nready;
}


void select_handle_accept(select_t *sel) {
    if(FD_ISSET(sel->listenfd_, &sel->rset_)) {
        int peerfd = accept(sel->listenfd_, NULL, NULL);
        if(peerfd == -1)
            ERR_EXIT("accept");
        select_add_fd(sel, peerfd);  // add
        --sel->nready_;
    }
}


void select_handle_data(select_t *sel) {
    if(sel->nready_ == 0)
        return ;
    int i;
    for(i = 0; i <= sel->maxi_; ++i) {
        if(FD_ISSET(sel->clients_[i], &sel->rset_)) {  // 存在准备好的客户fd
            int peerfd = sel->clients_[i];
            char recvbuf[1024];
            int ret = readline(peerfd, recvbuf, 1024);
            if(ret == -1) {
                ERR_EXIT("readline");
            } else if(ret == 0) {  
                printf("client close\n");
                select_del_fd(sel, i);  // 删掉已关闭的客户fd
                continue;
            } else {
                sel->handle_callback_(peerfd, recvbuf);  // 调用用户自定义的逻辑
            }
        }
    }
}


void select_add_fd(select_t *sel, int fd) {
    int i;
    // 找出最小的可用fd
    for(i = 0; i < FD_SETSIZE; ++i) {
        if(sel->clients_[i] == -1) {
            sel->clients_[i] = fd;
            if(i > sel->maxi_)
                sel->maxi_ = i;
            break;
        }
    }

    if(i == FD_SETSIZE) {
        fprintf(stderr, "too many clients\n");
        exit(EXIT_FAILURE);
    }
    FD_SET(fd, &sel->allset_);  // 更新备份
    if(fd > sel->maxfd_)        // 更新maxfd_
        sel->maxfd_ = fd;
}


void select_del_fd(select_t *sel, int i) {
    assert(i >= 0 && i < FD_SETSIZE);
    int fd = sel->clients_[i];
    sel->clients_[i] = -1;
    FD_CLR(fd, &sel->allset_);  // 更新备份
    close(fd);
}
