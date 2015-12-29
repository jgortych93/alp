#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */
#include <sys/ioctl.h>
#include "./net_mod.h"
int main()
{
int file_desc,ret_val;
char* message;
file_desc = open("/dev/ALP", 0);
if (file_desc < 0) 
	{
	
		printf("Can't open device file: ALP\n");
		exit(-1);
	}
	
ret_val = ioctl(file_desc, IOCTL_GET_IFS, message,3,4);
printf("%s",message);
close(file_desc);
	
return 0;
}