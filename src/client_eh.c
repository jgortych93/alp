#include "client_eh.h"
#include "ru.h"
#include "reactor.h"
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include "stdio.h"
#include "../Module/chardev.h"

static int handle_client_message(event_handler* self, struct message* m)
{
	int fd = self->fd;
	int result = -1;
	size_t len=0;
	while(1)
	{
	switch (m->x) {
			
		case ACK_NACK:
			{
				if ((send_ifs(fd))==-1)
				{
					printf("Send_ifs zwrocilo -1\n");
					exit(-1);
				}
				
				break;
			}
		case GET_INFO:
			{
				len=strlen("Wybierz interfejs\n")+1;
				send_bytes(fd,"Wybierz interfejs\n",len);
				break;
			}
		case SHOW_ALL:
			{
				if ((send_inf(fd,m->wiad))==-1)
				{
					printf("Send_inf zwrocilo -1\n");
					exit(-1);
				}
				
				break;
			}
		case SET_MAC_INTRO:
			{
				len=strlen("Wprowadz nowy adres MAC\n")+1;
				send_bytes(fd,"Wprowadz nowy adres MAC\n",len);
				break;
			}
		default:
			
			
			break;
	}

	delete_message(m);
	m = receive_message(self->fd);
	}
	return result;
}

static void serve_client(event_handler* self, uint32_t e)
{
	int result = -1;
	
	struct message* m = 0;
	
	if (e & EPOLLIN) {
		
		m = receive_message(self->fd);
		if (m)
			result = handle_client_message(self, m);
	}

	if (result < 0) 
	{
		self->ul->rm_user_by_fd(self->ul->ctx, self->fd);
		self->r->rm_eh(self->r, self->fd);
	}
	
}

event_handler* create_client_eh(int fd, reactor* r, user_list* ul)
{
	event_handler* eh = malloc(sizeof(event_handler));
	eh->fd = fd;
	eh->r = r;
	eh->ul = ul;
	eh->handle_event = &serve_client;

	return eh;
}

