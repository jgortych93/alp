
#ifndef CHARDEV_H
#define CHARDEV_H

#include <linux/ioctl.h>


#define MAJOR_NUM 100

//zapisuje w char* liste interfejsow sieciowych
#define IOCTL_GET_IFS	0
#define IOCTL_GET_INF	1

#define DEVICE_FILE_NAME "/dev/ALP"

#endif

struct cdev *kernel_cdev; 
struct device *chr_dev; 
static int Major;
static struct class *dev_Class; 