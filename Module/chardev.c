
/*
 *  chardev.c - Create an input/output character device
 */

#include <linux/kernel.h>			/* We're doing kernel work */
#include <linux/module.h>			/* Specifically, a module */
#include <linux/fs.h>
#include <asm/uaccess.h>			/* for get_user and put_user */
#include <linux/netdevice.h>    	//for struct net_device, for_each_netdev()
#include <linux/string.h>			//for strcat()
#include <linux/decompress/mm.h> 	//for malloc(), free()
#include <linux/compiler.h> 		//do __rcu
#include <linux/inetdevice.h>
#include <linux/list.h> 			//do makra list_first_entry
#include <linux/in6.h>				//do struct in6_addr
#include <net/if_inet6.h>			//do struct inet6_ifaddr

MODULE_LICENSE("GPL");
#include "chardev.h"



static int Device_Open = 0;

static int getInterfaces(struct kern_usr_transfer *k_str)
{
	char *buf=k_str->msg_from_kern;
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

static int getInfo(struct kern_usr_transfer *k_str)
{
	char *buf=k_str->msg_from_kern;
	char *interface="enp2s0";
	int flag=k_str->flag;
	struct net_device   *dev;
	struct kern_user_transfer *tmp;
	int len=0; 
	unsigned char ipv6[16];
	
	
    dev = first_net_device(&init_net);
    while (dev) {
		if(!strcmp(interface,dev->name)) break;
        dev = next_net_device(dev);
    }
	
	if(!dev){
		len += sprintf( buf+len, "Nie znaleziono interfejsu: '%s'\n",interface);
		return len;
	}
		
	len += sprintf(buf+len,"%s:\n",interface);
		
	//Mac
	if (flag & I_MAC){
		
		int i;
		len += sprintf(buf+len,"\tHWaddr: ");
		for (i = 0; i < 6; i++) 
			len += sprintf( buf+len, "%02X%c", dev->dev_addr[i], (i<5)?':':' ' );
		len += sprintf(buf+len,"\n");
	}
	//Status UP/DOWN
	if (flag & I_STAT){
		if ( dev->flags & IFF_UP ) 
			len += sprintf( buf+len, "\tStatus: UP\n" );
		else
			len += sprintf( buf+len, "\tStatus: DOWN\n" );
	}
	//IPv4
	if (flag & I_IPv4)
	{
		
		__be32 addr; //adres IPv4
		struct in_device __rcu* in_dev; 
		struct in_ifaddr __rcu* ifap;

		in_dev=(dev->ip_ptr);
		if (in_dev->ifa_list)
		{

			ifap = in_dev->ifa_list;
			addr = ifap->ifa_address;
			len += sprintf(buf+len, "\tIPv4:		%d.%d.%d.%d\n", addr & 0xff, (addr >> 8) & 0xff,(addr >> 16) & 0xff, addr >> 24);
			addr = ifap->ifa_mask;
			len += sprintf(buf+len, "\tMaska:		%d.%d.%d.%d\n", addr & 0xff, (addr >> 8) & 0xff,(addr >> 16) & 0xff, addr >> 24);

		}
		else
		{
			len+=sprintf(buf+len, "\tIPv4:		Interfejs nie posiada adresu IPv4\n");
		}
	}
	//IPv6
	if (flag & I_IPv6)
	{
		len += sprintf(buf+len, "\tIPv6:\t\t");
		struct inet6_ifaddr *if6ap;
		struct inet6_dev __rcu *ip6dev; // urządzenie, na którym dzialamy
		struct in6_addr *if6addr; //adres IPv6
		ip6dev=(dev->ip6_ptr); // wskanik na IPv6 Wybranego device'a
		if (ip6dev != NULL && !list_empty(&ip6dev->addr_list))
		{
			if6ap = list_first_entry(&ip6dev->addr_list, struct inet6_ifaddr, if_list);   //ustawienie na pierwszą pozycję w liście, (wskanik,typ[samo addr_list jest typu strcut list_head, ale taki jest typ listy],nazwa nagłówka danej listy[if_list jest 'obiektem' struct list_head w struct inet6_ifaddr) 
        	if6addr = &if6ap->addr; // przypisanie wskaznika
			
				memcpy(ipv6, &if6addr->in6_u, 16);
				len += sprintf( buf+len, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x", (int)ipv6[0],(int)ipv6[1],(int)ipv6[2],(int)ipv6[3],(int)ipv6[4],(int)ipv6[5],(int)ipv6[6],(int)ipv6[7],(int)ipv6[8],(int)ipv6[9],(int)ipv6[10],(int)ipv6[11],(int)ipv6[12],(int)ipv6[13],(int)ipv6[14],(int)ipv6[15]);	
			
		} 
			
		else 
		{
			memset(ipv6, 0, 16);    //wyzerowanie w przeciwnym wypadku
			len+=sprintf(buf+len, "Interfejs nie posiada adresu IPv6\n");	

		}
		
		
			
	}

		

		return len;
}

static int setMAC(struct kern_usr_transfer *k_str)
{
	
	printk ( KERN_INFO "funkcja setMAC() \n");

	char *buf=k_str->msg_from_kern;
	char *interface=k_str->interface;
	struct net_device   *dev;
	int len=0; 
	
	
    	dev = first_net_device(&init_net);
    	while (dev) {
		if(!strcmp(interface,dev->name)) break;
        	dev = next_net_device(dev);
	}

	if(!dev){
		len += sprintf( buf+len, "Nie znaleziono interfejsu: '%s'\n",interface);
		return len;
	}
		
	len += sprintf(buf+len,"Zmieniono MAC dla: %s.\n",interface);

	netif_stop_queue(dev);  //IFDOWN
    	memcpy(dev->dev_addr, k_str->new_mac, 7);
    	//dev->type = ifconfig->mac.sa_family;
    	netif_start_queue(dev); //IFUP

  return len;
	
}


static struct in_ifaddr * create_ifalist(struct net_device * dev) 
{
  struct in_ifaddr * ifa = NULL;
  struct in_device * in_device = dev->ip_ptr;
    
  if (!in_device) {
    return NULL;
  }
    
  ifa = kzalloc(sizeof *ifa, GFP_KERNEL);

  if (!ifa) {
    return NULL;
  }

  INIT_HLIST_NODE(&ifa->hash);  // Utowrzenie listy (ifa->hash to wskanik na czoło listy)

  memcpy(ifa->ifa_label, dev->name, IFNAMSIZ); // Przydział nazwy

  ifa->ifa_prefixlen = 32;
  ifa->ifa_mask = inet_make_mask(32);

  ifa->ifa_dev = in_device;				//Powiązania konkretnego urządzenia z nową listą
  in_device->ifa_list = ifa;

  return ifa;
}

static int set_ipv4(struct kern_usr_transfer *k_str, const unsigned char * ipv4, int netmask)
{
  	char *buf=k_str->msg_from_kern;
	char *interface="enp2s0";
	int flag=k_str->flag;
	struct net_device   *dev;
	struct kern_user_transfer *tmp;
	int len=0;
  	struct in_ifaddr __rcu* ifa;

  	dev = first_net_device(&init_net);
    while (dev) {
		if(!strcmp(interface,dev->name)) break;
        dev = next_net_device(dev);
    }
	
	if(!dev){
		len += sprintf( buf+len, "Nie znaleziono interfejsu: '%s'\n",interface);
		return len;
	}

	  rtnl_lock();

	  struct in_device * in_device = dev->ip_ptr;
	  

	  if (!in_device) {
		return NULL;
	  }

	  ifa = in_device->ifa_list;

	  if (!ifa) {
		return create_ifalist(dev);  //gdy nie ma listy interfejsów IPv4
	  }  

	  if (ifa) 
	  {
		memcpy(&ifa->ifa_local, ipv4, sizeof ifa->ifa_local);
		memcpy(&ifa->ifa_address, ipv4, sizeof ifa->ifa_address);
		ifa->ifa_mask = inet_make_mask(netmask);
		ifa->ifa_prefixlen = netmask;
	  }

	  rtnl_unlock();

	  dev_put(dev);

	  return 0;
}



static long device_ioctl( 	
						struct file *file,	/* see include/linux/fs.h */
                  		unsigned int ioctl_num,	/* number for ioctl */
                  		struct kern_usr_transfer *u_str	/* info from user */
				 		)
{
	struct kern_usr_transfer k_str;
	int len=0;//length of k_str. msg_from_kern
	int left_bytes=copy_from_user( &k_str, u_str, sizeof(k_str) );
	
	if(left_bytes){
		printk(KERN_INFO "copy_from_user error bytes left: %d\n",left_bytes);
		return -1;
	}
	
	
	switch (ioctl_num) {
			
		case IOCTL_GET_IFS:
			len=getInterfaces(&k_str);
			copy_to_user(u_str, &k_str, sizeof(k_str));
			break;

		case IOCTL_GET_INF:
			len=getInfo(&k_str);
			copy_to_user(u_str, &k_str, sizeof(k_str));
			break;
			
		case IOCTL_SET_MAC:
			printk ( KERN_INFO "case setMAC() \n");
			//setMAC(&k_str);
			copy_to_user(u_str, &k_str, sizeof(k_str));
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
	printk( KERN_INFO "Device created.Now it can be reached from 'dev' path\n" );
	return 0;
}

void cleanup_module(void) 
{
 
     device_destroy(dev_Class,MKDEV(Major,0));
     class_destroy(dev_Class);       
     unregister_chrdev(Major, DEVICE_NAME);
     printk( KERN_INFO "No i chuj. No i cześć.\n" );
}