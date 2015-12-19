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

extern errno;

int init_server(int* s, int* e);

int main(int argc, const char *argv[])
{
	int srv_fd = -1;
	int epoll_fd = -1;
	printf("luhunapocz");
	user_list* ul = 0;
	reactor* r = 0;
	event_handler* seh = 0;

	if (init_server(&srv_fd, &epoll_fd) != 0)
		return 1;

	ul = create_user_list();
	r  = create_reactor(epoll_fd);
	seh = create_acceptor(srv_fd, r, ul);
	r->add_eh(r, seh);

	r->event_loop(r);
	 
	return 0;
}

int init_server(int* s, int* e)
{
	int srv_fd = -1;
	int epoll_fd = -1;
	struct sockaddr_in srv_addr;
	struct epoll_event ee;

	memset(&srv_addr, 0, sizeof(struct sockaddr_in));
	memset(&ee, 0, sizeof(e));

	srv_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (srv_fd < 0) {
		printf("Cannot create socket\n");
		return 1;
	}

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(5557);
	if (bind(srv_fd, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0) {
		printf("Cannot bind socket\n");
		close(srv_fd);
		return 1;
	}

	if (listen(srv_fd, 1) < 0) {
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
		close(epoll_fd);
		close(srv_fd);
		return 1;
	}
	
	*s = srv_fd;
	*e = epoll_fd;
	return 0;
}

