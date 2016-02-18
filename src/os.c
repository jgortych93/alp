
#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>

#include "user_list.h"
#include "reactor.h"
#include "acceptor_eh.h"
#include "client_eh.h"

int  os_socket(int domain, int type, int protocol)
{
	return socket(domain, type, protocol);
}

int os_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	return bind(sockfd, addr, addrlen);
}

int os_listen(int sockfd, int backlog)
{
	return listen(sockfd, backlog);
}

int os_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return accept(sockfd, addr, addrlen);
}

ssize_t os_read(int fd, void *buf, size_t count)
{
	return read(fd, buf, count);
}

ssize_t os_write(int  fd,  const  void  *buf, size_t count)
{
	return write(fd,  buf, count);
}

int os_close(int fd)
{
	return close(fd);
}
int init_server(int* s, int* e)
{
	int srv_fd = -1;
	int epoll_fd = -1;
	struct sockaddr_in srv_addr;
	struct epoll_event ee;

	memset(&srv_addr, 0, sizeof(struct sockaddr_in));
	memset(&ee, 0, sizeof(e));

	srv_fd = os_socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (srv_fd < 0) {
		printf("Cannot create socket\n");
		return 1;
	}

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(5557);
	if (os_bind(srv_fd, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0) {
		printf("Cannot bind socket\n");
		close(srv_fd);
		return 1;
	}

	if (os_listen(srv_fd, 1) < 0) {
		printf("Cannot listen\n");
		close(srv_fd);
		return 1;
	}

	epoll_fd = epoll_create(MAX_USERS + 1);
	if (epoll_fd < 0) {
		printf("Cannot create epoll\n");
		close(srv_fd);
		return 1;
	}

	ee.events = EPOLLIN;
	ee.data.fd = srv_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, srv_fd, &ee) < 0) 
	{
		printf("Cannot add server socket to epoll\n");
		os_close(epoll_fd);
		os_close(srv_fd);
		return 1;
	}
	
	*s = srv_fd;
	*e = epoll_fd;
	return 0;
}
