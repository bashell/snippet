#include "network.h"

ssize_t readn(int fd, void *usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while(nleft > 0) {
        if((nread = read(fd, bufp, nleft)) == -1) {
            if(errno == EINTR)  // interrupt
                nread = 0;  // continue to read
            else
                return -1;  // ERROR
        } else if(nread == 0) {  // EOF
            break;
        } 
          
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);
}


ssize_t writen(int fd, void *usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nwrite;
    char *bufp = usrbuf;

    while(nleft > 0) {
        if((nwrite = write(fd, bufp, nleft)) == -1) {
            if(errno == EINTR)  // interrupt
                nwrite = 0;
            else
                return -1;  // ERROR
        } else if(nwrite == 0) {  // 这种情况也视为错误
            return -1;  
        }

        nleft -= nwrite;
        bufp += nwrite;
    }
    return n;
}


// recv_peek完成一次正确读取过程
ssize_t recv_peek(int sockfd, void *usrbuf, size_t n) {
    int nread;
    while(1) {
        // 只成功调用一次
        if((nread = recv(sockfd, usrbuf, n, MSG_PEEK)) == -1) {
            if(errno == EINTR)  // interrupt
                continue;
            else
                return -1;  // ERROR
        }
        break;
    }
    return nread;
}


ssize_t readline(int sockfd, void *usrbuf, size_t maxline) {
    int nread;      // 一次I/O读取的数量
    int nleft;      // 剩余字节数
    char *ptr;      // 存储usrbuf
    int ret;        // readn的返回值
    int total = 0;  // 当前总共读取的字节数

    nleft = maxline - 1;
    ptr = usrbuf;

    while(nleft > 0) {
        // 预览数据, 并没有真正取走
        ret = recv_peek(sockfd, ptr, nleft);
        if(ret <= 0)  // 读取的字节不够时也不是错误
            return ret;

        nread = ret;
        int i;
        for(i = 0; i < nread; ++i) {
            if(ptr[i] == '\n') {
                // 真正的读取过程
                ret = readn(sockfd, ptr, i+1);
                if(ret != i+1)
                    return -1;
                total += ret;
                ptr += ret;
                *ptr = 0;
                return total;  // 返回长度, 包括'\n'
            }
        }
        // 若执行到此, 则说明没有读取到'\n', 那么数据应该全部接收
        ret = readn(sockfd, ptr, nread);
        if(ret != nread)
            return -1;

        nleft -= nread;
        ptr += nread;
        total += nread;
    }
    *ptr = 0;
    return maxline - 1;
}
