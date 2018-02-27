/*
 * Aaron Chan
 * ECE 373 (Spring 2017)
 * Assignment #2
 * 
 * Char device that will retrieve
 * and write data to the kernel.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVCNT 5
#define DEVNAME "mycdev"

static struct mydev_dev {
	struct cdev cdev;
	int syscall_val;
} mydev;

static dev_t mydev_node;

/* this shows up under /sys/modules/mycdev/parameters */
static int exam = 15;
module_param(exam, int, S_IRUSR | S_IWUSR);

/* this doesn't appear in /sys/modules */
static int exam_nosysfs = 15;
module_param(exam_nosysfs, int, 0);

// Assignment #2 module param
static int syscall_val = 40;
module_param(syscall_val,int,S_IRUSR | S_IWUSR);

static int mycdev_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "(mycdev)successfully opened!\n");
	
	mydev.syscall_val = syscall_val;
	return 0;
}

static ssize_t mycdev_read(struct file *file, char __user *buf,
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

	if (copy_to_user(buf, &mydev.syscall_val, sizeof(int))) {
		ret = -EFAULT;
		goto out;
	}
	ret = sizeof(int);
	*offset += len;

	/* Good to go, so printk the thingy */
	printk(KERN_INFO "(mycdev)User got from us %d\n",mydev.syscall_val);

out:
	return ret;
}

static ssize_t mycdev_write(struct file *file, const char __user *buf,
                              size_t len, loff_t *offset)
{
	/* Have local kernel memory ready */
	char *kern_buf;
	int ret;
	int convert_success;
	/* Make sure our user isn't bad... */
	if (!buf) {
		ret = -EINVAL;
		goto out;
	}

	/* Get some memory to copy into... */
	kern_buf = kmalloc(len, GFP_KERNEL);

	/* ...and make sure it's good to go */
	if (!kern_buf) {
		ret = -ENOMEM;
		goto out;
	}

	/* Copy from the user-provided buffer */
	if (copy_from_user(kern_buf, buf, len)) {
		/* uh-oh... */
		ret = -EFAULT;
		goto mem_out;
	}

	ret = len;
	
	/* print what userspace gave us */
	printk(KERN_INFO "(mycdev)Userspace wrote \"%s\" to us\n", kern_buf);
	
	convert_success = kstrtol(kern_buf,10,&mydev.syscall_val);

	if(convert_success < 0)
		printk(KERN_INFO "(mycdev) Error converting values.\n");

	printk(KERN_INFO "(mycdev)syscall_val should be changed: %d\n",mydev.syscall_val);
	
mem_out:
	kfree(kern_buf);
out:
	return ret;
}

/* File operations for our device */
static struct file_operations mydev_fops = {
	.owner = THIS_MODULE,
	.open = mycdev_open,
	.read = mycdev_read,
	.write = mycdev_write,
};

static int __init mycdev_init(void)
{
	printk(KERN_INFO "(mycdev) module loading...\n", exam);

	if (alloc_chrdev_region(&mydev_node, 0, DEVCNT, DEVNAME)) {
		printk(KERN_ERR "alloc_chrdev_region() failed!\n");
		return -1;
	}

	printk(KERN_INFO "Allocated %d devices at major: %d\n", DEVCNT,
	       MAJOR(mydev_node));

	/* Initialize the character device and add it to the kernel */
	cdev_init(&mydev.cdev, &mydev_fops);
	mydev.cdev.owner = THIS_MODULE;

	if (cdev_add(&mydev.cdev, mydev_node, DEVCNT)) {
		printk(KERN_ERR "cdev_add() failed!\n");
		/* clean up chrdev allocation */
		unregister_chrdev_region(mydev_node, DEVCNT);

		return -1;
	}

	return 0;
}

static void __exit mycdev_exit(void)
{
	/* destroy the cdev */
	cdev_del(&mydev.cdev);

	/* clean up the devices */
	unregister_chrdev_region(mydev_node, DEVCNT);

	printk(KERN_INFO "(mycdev) module unloaded!\n");
}

MODULE_AUTHOR("Aaron Chan");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.2");
module_init(mycdev_init);
module_exit(mycdev_exit);
