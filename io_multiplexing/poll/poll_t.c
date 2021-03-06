#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "poll_t.h"
#include "network.h"



void poll_add_fd(poll_t *, int);
void poll_del_fd(poll_t *, int);


void poll_init(poll_t *pol, int listenfd, void (*handler)(int, char*)) {
    int i;
    for(i = 0; i < CLIENTSIZE; ++i)
        pol->clients_[i].fd = -1;
    pol->listenfd_ = listenfd;
    pol->maxi_ = 0;
    pol->nready_ = 0;
    pol->handle_callback_ = handler;

    pol->clients_[0].fd = listenfd;
    pol->clients_[0].events = POLLIN;
}


void poll_do_wait(poll_t *pol) {
    int nready;
    do {
        nready = poll(pol->clients_, pol->maxi_ + 1, -1);
    } while(nready == -1 && errno == EINTR);  // interrupt时继续等待
    if(nready == -1)  // 其他ERROR
        ERR_EXIT("poll");
    pol->nready_ = nready;
}


void poll_handle_accept(poll_t *pol) {
    if(pol->clients_[0].revents & POLLIN) {
        int peerfd = accept(pol->clients_[0].fd, NULL, NULL);
        if(peerfd == -1)
            ERR_EXIT("accept");
        poll_add_fd(pol, peerfd);  // add
    }
}


void poll_handle_data(poll_t *pol) {
    int i;
    char recvbuf[1024] = {0};
    for(i = 1; i <= pol->maxi_; ++i) {
        int peerfd = pol->clients_[i].fd;
        if(peerfd == -1)
            continue;
        if(pol->clients_[i].revents & POLLIN) {
            int ret = readline(peerfd, recvbuf, 1024);
            if(ret == -1)
                ERR_EXIT("readline");
            else if(ret == 0) {
                printf("client close\n");
                poll_del_fd(pol, i);  // del
                continue;
            } else {
                pol->handle_callback_(peerfd, recvbuf);
            }
        }
    }

}


void poll_add_fd(poll_t *pol, int fd) {
    int i;
    for(i = 1; i < CLIENTSIZE; ++i) {
        if(pol->clients_[i].fd == -1) {
            pol->clients_[i].fd = fd;
            pol->clients_[i].events = POLLIN;
            if(i > pol->maxi_)
                pol->maxi_ = i;
            break;
        }
    }
    if(CLIENTSIZE == i) {
        fprintf(stderr, "too many clients\n");
        exit(EXIT_FAILURE);
    }
}


void poll_del_fd(poll_t *pol, int i) {
    assert(i >= 1 && i < CLIENTSIZE);
    close(pol->clients_[i].fd);
    pol->clients_[i].fd = -1;
}
