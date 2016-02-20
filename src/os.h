#include <stdlib.h>
#include <arpa/inet.h>

int  os_socket(int domain, int type, int protocol);
int os_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int os_listen(int sockfd, int backlog);
int os_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int os_close(int fd);
int init_server(int* s, int* e);
