/*
 * Aaron Chan
 * ECE 373 (Spring 2017)
 * Assignment #4
 * 
 * This module will initialize a PCI driver
 * so that we can blink LEDs on the Atom Box.
 * We will set a default blinking rate and include
 * the capability to change the blink rate with
 * values passed in from userspace.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/pci.h>
#include <linux/time.h>

#define DEVCNT 5
#define DEVNAME "my_timer_blink"
#define DEV_NODE_NAME "hw4_led"

#define LED_ON 78   //LED0 on value
#define LED_OFF 15  //LED0 off value

#define VENDOR_ID 0x8086
#define DEVICE_ID 0x150c
#define LED_REG   0x0E00 // LED control register offset

// =========== Global ===================
static struct mydev_dev {
	struct cdev cdev;
    int input;    // store value for LED register
    bool status;     // flag for LED, (on or off)
    void *hw_addr;  // base address of driver   
} mydev;

static dev_t mydev_node;
static struct class *cl;    // device class
struct timer_list blinkLED; 
//static char *device_name = "my_pci_dev";

static int blink_rate = 2;
module_param(blink_rate, int, S_IRUSR | S_IWUSR);

//========= Functions ====================
// Called by timer. Turn LED0 on and off by timer intervals
static void timer_blink(unsigned long data)
{
    // Turn LED0 on or off for blink
    if(mydev.status)
    {
        mydev.status = false;
        writel((unsigned int)LED_OFF,mydev.hw_addr + LED_REG);
    }
    else
    {
        mydev.status = true;
        writel((unsigned int)LED_ON,mydev.hw_addr + LED_REG);
    }

    // restart timer
    mod_timer(&blinkLED, (HZ/blink_rate)+jiffies);

}

// Devices supported by this driver
static DEFINE_PCI_DEVICE_TABLE(pci_test_tbl) = {
	{ PCI_DEVICE(VENDOR_ID, DEVICE_ID) },
	{ }, /* must have an empty at the end! */
};	

// Enable PCI device and map to memory
static int my_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    resource_size_t mmio_start, mmio_len;
    int bars, err;

	/* this is where I'd map BAR's for access, save stuff off, etc. */
	printk(KERN_INFO "It's dangerous to go alone, take this with you.\n");

    err = pci_enable_device_mem(pdev);
		
    // set up pci pci connections
    bars = pci_select_bars(pdev,IORESOURCE_MEM);
	err = pci_request_selected_regions(pdev,bars,DEVNAME);

	pci_set_master(pdev);
    
    // map memory
    mmio_start = pci_resource_start(pdev, 0);
    mmio_len = pci_resource_len(pdev,0);
    mydev.hw_addr = ioremap(mmio_start,mmio_len);

 	/* 0 means success */
	return 0;

}

// Clean up PCI allocations, disable device
static void my_pci_remove(struct pci_dev *pdev)
{
    iounmap(mydev.hw_addr);
    pci_release_selected_regions(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
    pci_disable_device(pdev);

    printk(KERN_INFO "So long!!\n");
}

// Information about my PCI driver
static struct pci_driver my_pci_driver = {
	.name = DEVNAME,
	.id_table = pci_test_tbl,
	.probe = my_pci_probe,
	.remove = my_pci_remove,
};

// Open function
static int pci_hw4_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "(my_pci_driver)successfully opened!\n");
    // Initialize and turn on LED when userspace opens file 
    mydev.status = true;
    writel((unsigned int)LED_ON,mydev.hw_addr + LED_REG);

    // Start timer for blinking
    mod_timer(&blinkLED, (HZ/blink_rate)+jiffies);
    return 0;
}

// Release function
static int pci_hw4_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "(my_pci_driver)successfully closed!\n");
    
    // Turn off LED when userspace closes file
    mydev.status = false;
    writel((unsigned int)LED_OFF,mydev.hw_addr + LED_REG);

    // Stop and remove timer
    del_timer_sync(&blinkLED);
    return 0;
}
// Read function
static ssize_t pci_hw4_read(struct file *file, char __user *buf,
                             size_t len, loff_t *offset)
{
	/* Get a local kernel buffer set aside */
	int ret;

	if (*offset >= sizeof(int))
		return 0;

	/* Make sure our user wasn't bad... */
	if (!buf) {
		ret = -EINVAL;
		goto out;
	}

    // Pass blink rate value to userspace
	if (copy_to_user(buf, &blink_rate, sizeof(unsigned int))) {
		ret = -EFAULT;
		goto out;
	}
	ret = sizeof(unsigned int);
	*offset += len;

	/* Good to go, so printk the thingy */
	printk(KERN_INFO "(my_pci_driver:read)User got from us %d\n",blink_rate);

out:
	return ret;
}

// Write function
static ssize_t pci_hw4_write(struct file *file, const char __user *buf,
                              size_t len, loff_t *offset)
{
	int ret;
    /* Make sure our user isn't bad... */
	if (!buf) {
		ret = -EINVAL;
		goto out;
	}
	
	/* Copy from the user-provided buffer */
	if (copy_from_user(&mydev.input, buf, len)) {
		/* uh-oh... */
		ret = -EFAULT;
		goto out;
	}
    
    if(mydev.input < 0)
    {
       printk("(my_pci_driver:write)User wrote negative value. Return error\n");
       ret = EINVAL;
        goto out;
    }
    else if(mydev.input == 0)
        printk("(my_pci_driver:write)User wrote 0. Do nothing\n");
    else
    {
        printk("(my_pci_driver:write)User wrote %d\n",mydev.input);
        blink_rate = mydev.input;
    }
 
	ret = len;

out:
	return ret;
}

/* File operations for our device */
static struct file_operations mydev_fops = {
	.owner = THIS_MODULE,
	.open = pci_hw4_open,
	.read = pci_hw4_read,
	.write = pci_hw4_write,
    .release = pci_hw4_release,
};

// Initialization
static int __init pci_hw4_init(void)
{
    int ret;
    mydev.status = false;
    printk(KERN_INFO "(my_pci_driver) Registering PCI Driver...\n");            
    ret = pci_register_driver(&my_pci_driver);

    printk(KERN_INFO "(my_pci_driver) module loading...\n");

	if (alloc_chrdev_region(&mydev_node, 0, DEVCNT, DEVNAME)) {
		printk(KERN_ERR "alloc_chrdev_region() failed!\n");
		return -1;
	}

    // Get major number for device
	printk(KERN_INFO "Allocated %d devices at major: %d\n", DEVCNT,
	       MAJOR(mydev_node));
    
    // Create node file. No need for mknod
    if((cl = class_create( THIS_MODULE, DEVNAME)) == NULL)
    {
        printk(KERN_ALERT "Class creation failed\n");
        unregister_chrdev_region(mydev_node,DEVCNT);
        return -1;
    }
    if(device_create(cl, NULL, mydev_node, NULL, DEV_NODE_NAME) == NULL)
    {
        printk(KERN_ALERT "Device creation failed\n");
        class_destroy(cl);
        unregister_chrdev_region(mydev_node,DEVCNT);
    }

	/* Initialize the character device and add it to the kernel */
	cdev_init(&mydev.cdev, &mydev_fops);
	mydev.cdev.owner = THIS_MODULE;

	if (cdev_add(&mydev.cdev, mydev_node, DEVCNT)) {
		printk(KERN_ERR "cdev_add() failed!\n");
		/* clean up chrdev allocation */
		unregister_chrdev_region(mydev_node, DEVCNT);

		return -1;
	}
    
    // Setup timer and start timer.
    setup_timer(&blinkLED, timer_blink,0);
    return ret;
}

// Clean up when removing driver
static void __exit pci_hw4_exit(void)
{
	/* destroy the cdev */
	cdev_del(&mydev.cdev);
    device_destroy(cl,mydev_node);
    class_destroy(cl);

    /* Unregister PCI Driver*/
    pci_unregister_driver(&my_pci_driver);

    // Get rid of timer
    del_timer_sync(&blinkLED);

	/* clean up the devices */
    unregister_chrdev_region(mydev_node, DEVCNT);
    printk(KERN_INFO "(my_pci_driver) module unloaded!\n");
}

MODULE_AUTHOR("Aaron Chan");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.2");
module_init(pci_hw4_init);
module_exit(pci_hw4_exit);
