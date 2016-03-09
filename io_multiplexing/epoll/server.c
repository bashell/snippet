#include "network.h"
#include "epoll_t.h"


void handler(int fd, char *buf) {
    printf("recv data: %s", buf);
    writen(fd, buf, strlen(buf));
}


int get_listen_fd() {
    // socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd == -1)
        ERR_EXIT("socket");

    // 端口复用
    int reuse = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        ERR_EXIT("setsockopt");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
        ERR_EXIT("bind");

    // listen
    if(listen(listenfd, SOMAXCONN) == -1)
        ERR_EXIT("listen");

    return listenfd;
}


int main(int argc, char *argv[])
{
    if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        ERR_EXIT("signal");

    int listenfd = get_listen_fd();

    epoll_t epoll;
    epoll_init(&epoll, listenfd, handler);
    while(1) {
        epoll_do_wait(&epoll);
        epoll_handle(&epoll);
    }
    epoll_close(&epoll);
    close(listenfd);
    return 0;
}
