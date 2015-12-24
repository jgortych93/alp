#ifndef CHARDEV_H
#define CHARDEV_H
#define _BSD_SOURCE
#define MAJOR_NUM 245

#define IOCTL_GET_IFS _IOR(MAJOR_NUM, 1, char *)
#define IOCTL_SET_MSG _IOR(MAJOR_NUM, 0, char *)

#include <linux/ioctl.h>


#endif
struct net_device* card;