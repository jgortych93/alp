#include <linux/kernel.h>	
#include <linux/module.h>	
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/unistd.h>
#include <linux/semaphore.h>
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

static int device_release(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "device_release(%p,%p)\n", inode, file);
#endif

	
	Device_Open--;

	module_put(THIS_MODULE);
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
	.release = device_release,	/* a.k.a. close */
};

int init_module()
{
	struct device *chr_dev =NULL ;  
	Major = register_chrdev(0, DEVICE_NAME, &Fops);
	if (Major < 0) 
	{
	     printk ("Registering the character device failed with %d\n", Major);
	     return Major;
	}
	
	printk("<1>I was assigned major number %d.\n", Major);
	 
	dev_Class = class_create(THIS_MODULE,DEVICE_NAME); 
	
	if( dev_Class == NULL)
	{
		printk( KERN_ALERT "Error!Class couldn't be created!\n" );
		return -1 ;
	}
	printk( KERN_INFO "Class created!\n" );

	chr_dev = device_create( dev_Class , NULL , MKDEV(Major,0),NULL,"ALP");
	
	if( chr_dev == NULL ) 
	{
		printk( KERN_ALERT "Error!Device file couldnt be created\n" );
		return -1;
	}
	printk( KERN_INFO "Device created.Now it can be reached from \dev path\n" );
	return 0;
}

void cleanup_module(void) 
{
 
     device_destroy(dev_Class,MKDEV(Major,0));
     class_destroy(dev_Class);       
     unregister_chrdev(Major, DEVICE_NAME);
     printk( KERN_INFO "No i chuj. No i cześć.\n" );
}