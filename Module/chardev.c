
/*
 *  chardev.c - Create an input/output character device
 */

#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/fs.h>
#include <asm/uaccess.h>	/* for get_user and put_user */
#include <linux/netdevice.h>    //for struct net_device, for_each_netdev()
#include <linux/string.h>	//for strcat()
#include <linux/decompress/mm.h> //for malloc(), free()
#include <linux/compiler.h>//do __rcu
#include <linux/inetdevice.h>
MODULE_LICENSE("GPL");
#include "chardev.h"

#define DEVICE_NAME "ALP"
#define MAJOR_NUM 100
#define BUF_SIZE 100

static int Device_Open = 0;

static int getInterfaces(char * buf)
{
	struct net_device   *dev;
	
    int nr=0;
	int len=0;

	dev = first_net_device(&init_net);
    while (dev) {
        nr++;
        len+=sprintf( buf+len, "%d.   %s\n", nr, dev->name );
        dev = next_net_device(dev);
    }
    return len;
}

static int getInfo(char* buf, char* interface, int info)
{
	struct net_device   *dev;
	int len=0; 
	int i;
	
	__be32 addr; //adres IPv4
	struct in_device __rcu* in_dev; 
	struct in_ifaddr __rcu* ifap;
	
    dev = first_net_device(&init_net);
    while (dev) {
		if(!strcmp(interface,dev->name)) break;
        dev = next_net_device(dev);
    }
	
	if(!dev){
		len += sprintf( buf+len, "Nie znaleziono interfejsu: '%s'\n",interface);
	}
	else{
		
		
		len += sprintf(buf+len,"%s:\n",interface);
		//Mac
		len += sprintf(buf+len,"\tHWaddr: ");
		for (i = 0; i < 6; i++) 
		    len += sprintf( buf+len, "%02X%c", dev->dev_addr[i], (i<5)?':':' ' );
		len += sprintf(buf+len,"\n");
		
		//Status UP/DOWN
		if ( dev->flags & IFF_UP ) 
			len += sprintf( buf+len, "\tStatus: UP\n" );
		else
			len += sprintf( buf+len, "\tStatus: DOWN\n" );
		
		//IP V4
		in_dev=(dev->ip_ptr);
		if (in_dev->ifa_list)
		{
			for (ifap = in_dev->ifa_list; ifap != NULL; ifap = ifap->ifa_next) 
			{
				addr=ifap->ifa_address;
				len+=sprintf(buf+len, "\tIPv4:   %d.%d.%d.%d\n", addr & 0xff, (addr >> 8) & 0xff,(addr >> 16) & 0xff, addr >> 24);
				addr=ifap->ifa_mask;
				len+=sprintf(buf+len, "\tMaska:   %d.%d.%d.%d\n", addr & 0xff, (addr >> 8) & 0xff,(addr >> 16) & 0xff, addr >> 24);
			}
		}
		else
		{
			len+=sprintf(buf+len, "\tIPv4:   Interfejs nie posiada adresu IPv4\n");
		}
		//Another shit
	}
    return len;
}

static long device_ioctl(struct file *file,	/* see include/linux/fs.h */
                  	unsigned int ioctl_num,	/* number for ioctl */
                  	char* buf_u,				/* bufor od usera, tutaj zapiszemy wiadomosc */
				 	char* ifc_name_u,		/* nazwa interfejsu */
				 	int anotherp
				 )
{

	char buf_k[BUF_SIZE];//tutj zapiszemy wiadomosc dla usera
	int len=0;//dlugosc naszej wiadomosci
	
	char ifc_name_k[BUF_SIZE];
	strnlen_user( ifc_name_u, BUF_SIZE );
	strncpy_from_user( ifc_name_k, ifc_name_u, BUF_SIZE );
	
	printk(KERN_INFO " Interfejs: %s\n",ifc_name_k);
	
	switch (ioctl_num) {
			
		case IOCTL_GET_IFS:
			len=getInterfaces(buf_k);
			clear_user(buf_u,len);
			copy_to_user(buf_u,buf_k,len+1);
			//put_user('\0',  buf_u + len + 1);
			break;

		case IOCTL_GET_INF:
			len=getInfo(buf_k, "wlp3s0", 0);
			clear_user(buf_u,len);
			copy_to_user(buf_u,buf_k,len+1);
			//put_user('\0',  buf_u + len + 1);
			break;
	}

	return 0;
}

/* 
 * This is called whenever a process attempts to open the device file 
 */
static int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "device_open(%p)\n", file);

	//We don't want to talk to two processes at the same time 
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	
	try_module_get(THIS_MODULE);
	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "device_release(%p,%p)\n", inode, file);

	//We're now ready for our next caller 
	Device_Open--;
	module_put(THIS_MODULE);
	return 0;
}

struct file_operations Fops = {
	.unlocked_ioctl = device_ioctl,
	.open = device_open,
	.release = device_release,	/* a.k.a. close */
};

/* 
 * Initialize the module - Register the character device 
 */
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
