#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "epoll_t.h"
#include "network.h"


void epoll_add_fd(epoll_t *epoll, int fd);
void epoll_del_fd(epoll_t *epoll, int fd);



void epoll_init(epoll_t *epoll, int listenfd, void (*handler)(int, char*)) {
    epoll->epollfd_ = epoll_create(CLIENTSIZE);
    if(epoll->epollfd_ == -1)
        ERR_EXIT("epoll_create");
    epoll->listenfd_ = listenfd;
    epoll_add_fd(epoll, listenfd);
    epoll->nready_ = 0;
    epoll->handle_callback_ = handler;
}


void epoll_do_wait(epoll_t *epoll) {
    int nready;
    do {
        nready = epoll_wait(epoll->epollfd_, epoll->events_, CLIENTSIZE, -1);
    } while(nready == -1 && errno == EINTR);  // interrupt时继续
    if(nready == -1)  // 其他ERROR
        ERR_EXIT("epoll_wait");
    epoll->nready_ = nready;
}


void epoll_handle(epoll_t *epoll) {
    int i;
    for(i = 0; i < epoll->nready_; ++i) {
        int fd = epoll->events_[i].data.fd;
        if(fd == epoll->listenfd_)
            epoll_handle_accept(epoll);
        else
            epoll_handle_data(epoll, fd);
    }
}


void epoll_handle_accept(epoll_t *epoll) {
    int peerfd = accept(epoll->listenfd_, NULL, NULL);
    if(peerfd == -1)
        ERR_EXIT("accept");
    epoll_add_fd(epoll, peerfd);  // add
}


void epoll_handle_data(epoll_t *epoll, int peerfd) {
    char recvbuf[1024] = {0};
    int ret = readline(peerfd, recvbuf, 1024);
    if(ret == -1)
        ERR_EXIT("readline");
    else if(ret == 0) {
        printf("cilent close\n");
        epoll_del_fd(epoll, peerfd);  // del
        return ;
    } else {
        epoll->handle_callback_(peerfd, recvbuf);
    }
}


void epoll_close(epoll_t *epoll) {
    close(epoll->epollfd_);
    close(epoll->listenfd_);
}


void epoll_add_fd(epoll_t *epoll, int fd) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if(epoll_ctl(epoll->epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)
        ERR_EXIT("epoll_ctl");
}


void epoll_del_fd(epoll_t *epoll, int fd) {
    struct epoll_event ev;
    ev.data.fd = fd;
    if(epoll_ctl(epoll->epollfd_, EPOLL_CTL_DEL, fd, &ev) == -1)
        ERR_EXIT("epoll_ctl");
}
