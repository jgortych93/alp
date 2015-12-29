/*
 *  ioctl.c - the process to use ioctl's to control the kernel module
 *
 *  Until now we could have used cat for input and output.  But now
 *  we need to do ioctl's, which require writing our own process.
 */

/* 
 * device specifics, such as ioctl numbers and the
 * major device file. 
 */
#include "chardev.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */
#include <sys/ioctl.h>		/* ioctl */
#include <errno.h>
#define ROZ 100
extern int errno;

main()
{
	int file_desc, ret_val;
	char msg[ROZ];
	

	file_desc = open(DEVICE_FILE_NAME, 0);
	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}
	char* inter="eth0";
	
	char msg2[ROZ];
	
	ret_val = ioctl(file_desc, IOCTL_GET_IFS, msg2,9,4);
	if ((ret_val)<0)
		printf("Pierwszy ioctl: %s",strerror(errno));
	printf("%s\n",msg2);
	
	
	
	ret_val = ioctl(file_desc, IOCTL_GET_INF, msg,9,4);
	if ((ret_val)<0)
		printf("%s",strerror(errno));
	printf("%s\n",msg);
	close(file_desc);
}
