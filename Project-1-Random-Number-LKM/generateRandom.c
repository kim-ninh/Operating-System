#include <linux/init.h>			// __init __exit
#include <linux/module.h>		// Core header
#include <linux/kernel.h>		// Type, macros,function for kernel
#include <linux/random.h>		// get_random_bytes()
#include <linux/device.h>		// Kernel Driver Model
#include <linux/fs.h>			// Linux file system
#include <linux/uaccess.h>		// Copy to user function
#include <linux/mutex.h>
#define DEVICE_NAME "grnchar"
#define CLASS_NAME "grn"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kim Ninh");
MODULE_DESCRIPTION("A simple Linux drive for generate random number");
MODULE_VERSION("0.1");

static DEFINE_MUTEX(grnchar_mutex);
static int randomNumber = 0;
static int majorNumber;
static struct class* ubbcharClass = NULL;
static struct device* ubbcharDevice = NULL;

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.release = dev_release,
};

static int __init random_init(void){
	printk(KERN_INFO "GRNChar: Initializing the GRNChar LKM\n");
	
	//Phat sinh 1 so nguyen 4 byte
	get_random_bytes(&randomNumber, sizeof(randomNumber));
	printk(KERN_INFO "GRNChar: The random number is: %d", randomNumber);
	
	// Thu lay majorNumber 1 cach tu dong
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0){
      printk(KERN_ALERT "GRNChar failed to register a major number\n");
      return majorNumber;
    }
    printk(KERN_INFO "GRNChar: registered correctly with major number %d\n", majorNumber);
	
	// Register the device class
   ubbcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(ubbcharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(ubbcharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "GRNChar: device class registered correctly\n");
	
	 // Register the device driver
   ubbcharDevice = device_create(ubbcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(ubbcharDevice)){               // Clean up if there is an error
      class_destroy(ubbcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(ubbcharDevice);
   }
   printk(KERN_INFO "GRNChar: device class created correctly\n"); // Made it! device was initialized
   
   mutex_init(&grnchar_mutex);
   return 0;	
}

static void __exit random_exit(void){
	mutex_destroy(&grnchar_mutex);
	device_destroy(ubbcharClass, MKDEV(majorNumber, 0));
	class_unregister(ubbcharClass);
	class_destroy(ubbcharClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO "GRNChar: Goodbye from the LKM\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
	if(!mutex_trylock(&grnchar_mutex)){
		printk(KERN_ALERT "GRNChar: Device in use by another process");
		return -EBUSY;
	}
	printk(KERN_INFO "GRNChar: Device has been opened\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int error_count = 0;
	
	error_count = copy_to_user(buffer,(char*) &randomNumber, sizeof(randomNumber));
	
	if (error_count == 0){
		printk(KERN_INFO "GRNChar: Sent number %d to the user\n", randomNumber);
		return 0;
	}
	
	printk(KERN_INFO "GRNChar: Failed to sent number %d to the user\n", randomNumber);
	return -EFAULT;
}

static int dev_release(struct inode *inodep, struct file *filep){
	mutex_unlock(&grnchar_mutex);
	printk(KERN_INFO "GRNChar: Device successfully closed\n");
	return 0;
}

module_init(random_init);
module_exit(random_exit);
