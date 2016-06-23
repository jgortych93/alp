#include "ru.h"
#include <unistd.h>
#include <string.h>
#include "../Module/chardev.h"
#include <errno.h>

int send_bytes(int fd, const char* msg, size_t len)
{
	int result = -1;
	if (write(fd, &len, sizeof(size_t)) > 0)
		if (write(fd, msg, len) == len)
			result = 0;

	return result;
}


struct message* receive_message(int fd)
{
	size_t len = 0;
	char* msg = 0;
	struct message* m = 0;

	if (read(fd, &len, sizeof(size_t)) < 1)
		return 0;

	msg = malloc(len * sizeof(char));
	if (read(fd, msg, len) != len) {
		return 0;
	}

	m = malloc(sizeof(struct message));
	switch (msg[0]) 
	{
		
		case '1':
			m->x = ACK_NACK;
			break;
		case '2':
			m->x = GET_INFO;
			break;
		case '3':
			m->x = SET_MAC_INTRO;
		case 'A':
			m->x= SHOW_ALL;
			m->wiad="ALL";
			break;
			
	}

	free(msg);
	return m;
}

void delete_message(struct message* m)
{
	free(m);
}


int send_ifs(int fd)
{
	size_t len;
	int file_desc, ret_val;
	char msg2[ROZ];
	file_desc = open(DEVICE_FILE_NAME, 0);
	if (file_desc < 0) {
	printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
	exit(-1);
	}
	ret_val = ioctl(file_desc, IOCTL_GET_IFS, msg2);
	if ((ret_val)<0)
	{
		printf("Pierwszy ioctl: %s",strerror(errno));
		return -1;
	}
	len=(strlen(msg2))+1;
	if((send_bytes(fd,msg2,len))==-1)
		return -1;
	close(file_desc);
	
}
int send_inf(int fd,const char* interfejs)
{
	size_t len;
	int file_desc, ret_val;
	
	char msg2[ROZ];
	file_desc = open(DEVICE_FILE_NAME, 0);
	if (file_desc < 0) {
	printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
	exit(-1);
	}
	ret_val = ioctl(file_desc, IOCTL_GET_INF, msg2);
	if ((ret_val)<0)
	{
		printf("Pierwszy ioctl: %s",strerror(errno));
		return -1;
	}
	len=(strlen(msg2))+1;
	if((send_bytes(fd,msg2,len))==-1)
		return -1;
	close(file_desc);
	
}