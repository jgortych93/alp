#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

int main(int argc, char** argv)
{
	int s_fd,c_fd;
	struct sockaddr_in serv;
	serv.sin_family=AF_INET;
	serv.sin_addr.s_addr=htonl(INADDR_ANY);
	
	
	s_fd = socket(PF_INET,SOCK_STREAM,0);
	bind(s_fd,(struct sockaddr*)&serv,sizeof(struct sockaddr_in));
	
	listen (s_fd,5);

	return 0;
}


