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
#include "os.h"
#include "serwer.h"




int main(int argc, const char *argv[])
{
	int wynik=serwer();
	 
	return wynik;
}


