#ifndef RSOCKET_H
#define RSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCK_MRP 1  // Custom socket type for MRP sockets

#define T 2  // Value of T (timeout interval) in seconds
#define P 0.2  // Value of p (probability of message drop)

// Function prototypes
int r_socket(int domain, int type, int protocol);
int r_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int r_sendto(int sockfd, const void *buf, size_t len, int flags,
             const struct sockaddr *dest_addr, socklen_t addrlen);
int r_recvfrom(int sockfd, void *buf, size_t len, int flags,
               struct sockaddr *src_addr, socklen_t *addrlen);
int r_close(int sockfd);
int dropMessage(float p);

#endif
