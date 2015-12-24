#include <linux/kernel.h>	
#include <linux/module.h>	
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/unistd.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include "./net_mod.h"

#define SUCCESS 0
#define DEVICE_NAME "char_dev"
MODULE_LICENSE("GPL");
static int Device_Open = 0;
static char* Message_Ptr;

static int device_open(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "device_open(%p)\n", file);
#endif

	
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	printk(KERN_INFO "device_open(%p)\n", file);

	try_module_get(THIS_MODULE);
	return SUCCESS;
}


static ssize_t device_read (struct file* file,char __user * buffer,size_t length,int option, loff_t * offset)
{
	int bytes_read = 0;//aktualnie zliczona libcza odczytanych bitów
	switch(option)
	{
		case 1:
			printk(KERN_INFO "device_open\n");
		case 2:
			printk(KERN_INFO "O kurwa. To działa.\n");
			/*if (*Message_Ptr == 0)
				return 0;
			while (length && *Message_Ptr) 
			{

		
			put_user(*(Message_Ptr++), buffer++);
			length--;
			bytes_read++;
			}
*/
			
	}
}

long device_ioctl(struct file *file,unsigned int ioctl_num,unsigned long ioctl_param)
{
	int i;
	char *temp;
	char ch;
	printk(KERN_INFO "Jestem w ioctl frajerzy");

	switch (ioctl_num) 
	{
		case IOCTL_GET_IFS:
			{
				i = device_read(file, (char*)ioctl_param, 99,1, 0);
				if (i<=0)
				{
					printk(KERN_ALERT "Niepoprawne sczytanie listy interfejsów");
					return -1;
				}	
				if((put_user('\0', (char*)ioctl_param + i))!=0)
				{
					printk(KERN_ALERT "Niepoprawne dodanie znaku końca");
					return -1;
				}	  
				break;
				  
			}
			
		
	}
	return SUCCESS;
}

struct file_operations Fops = {
	.read = device_read,
	.write = NULL,
	.unlocked_ioctl = device_ioctl,
	.open = device_open,
	.release = NULL,	/* a.k.a. close */
};

int init_module()
{
	int ret_val;
	dev_t* first,dev;
	struct class* cl;
	struct cdev *  	c_dev;
	/* 
	 * Register the character device (atleast try) 
	 */
	ret_val = register_chrdev(0,DEVICE_NAME,&Fops);
	printk(KERN_INFO " Registered with major number %d\n",ret_val);
	
	
	// * Negative values signify an error 
	 
	if (ret_val < 0) 
	{
		printk(KERN_ALERT "%s failed with %d\n",
		       "Sorry, registering the character device ", ret_val);
		return ret_val;
	}
	return 0;
	/*
	 printk(KERN_INFO "Welcome!");
    if (alloc_chrdev_region(&first, 0, 1, "char_dev") < 0)  //$cat /proc/devices
    {
        return -1;
    }
    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)    //$ls /sys/class
    {
        unregister_chrdev_region(first, 1);
        return -1;
    }
    
    cdev_init(&c_dev, &Fops);
    if (cdev_add(&c_dev, first, 1) == -1)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }

	if (device_create(cl, NULL, first, NULL, "mynull") == NULL) //$ls /dev/
    {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }*/
}