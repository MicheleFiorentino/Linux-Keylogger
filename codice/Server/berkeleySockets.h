#ifndef BERKELEYSOCKETS_H_INCLUDED
#define BERKELEYSOCKETS_H_INCLUDED

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>


/* BERKELEY WRAPPERS */

int Socket(int domain, int type, int protocol){
    int fd = socket(domain, type, protocol);
    if(fd<0){
        fprintf(stderr,"socket error\n");
        exit(1);
    }
    return fd;
}

void Connect(int fd, struct sockaddr_in addr, socklen_t addrlen){
    int status = connect(fd, (struct sockaddr*)&addr, addrlen);
    if(status<0){
        fprintf(stderr,"connect error\n");
        exit(2);
    }
}

void Bind(int fd, struct sockaddr_in addr, socklen_t addrlen){
    int status = bind(fd, (struct sockaddr*)&addr, addrlen);
    if(status<0){
        fprintf(stderr,"bind error\n");
        exit(3);
    }
}

void Listen(int fd, int backlog){
    int status = listen(fd, backlog);
    if(status < 0){
        fprintf(stderr,"listen error\n");
        exit(4);
    }
}

int Accept(int fd, struct sockaddr *addr, socklen_t *addrlen){
    int connfd = accept(fd, addr, addrlen);
    if(connfd < 0){
        fprintf(stderr, "accept error\n");
        exit(5);
    }
    return connfd;
}

void Close(int fd) {
    if (close(fd) < 0) {
        perror("close error\n");
        exit(6);
    }
}



/* FULLWRITE AND FULLREAD */

ssize_t fullWrite(int fd, const void *buf, size_t count){

    ssize_t nwritten;
    size_t nleft = count;
    while(nleft > 0){
        if( (nwritten = write(fd, buf, nleft)) < 0 ){
            if(errno==EINTR){
                continue;
            } else {
                exit(nwritten);
            }
        }
        nleft -= nwritten;
        buf += nwritten;
    }
    return nleft;
}

ssize_t fullRead(int fd, void *buf, size_t count){

    ssize_t nread;
    size_t nleft = count;
    while(nleft > 0){
        if( (nread = read(fd, buf, nleft)) < 0 ){
            if(errno==EINTR){
                continue;
            } else {
                exit(nread);
            }
        } else if(nread==0){   //EOF
            break;
        }
        nleft -= nread;
        buf += nread;
    }
    buf = 0;
    return(nleft);
}



/* OTHER */

// Initializes a struct sockaddr_in addr and returns it
struct sockaddr_in getInitAddr(int family, unsigned short port, char* IP){
    struct sockaddr_in addr;
    memset((void *)&addr, 0, sizeof(addr)); //clean addr
    addr.sin_family = family;
    addr.sin_port = htons(port);
    if(inet_pton(family, IP, &addr.sin_addr) < 0){
        fprintf(stderr, "inet_pton error for %s\n", IP);
        exit(-1);
    }
    return addr;
}



#endif