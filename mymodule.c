#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>


#include "main.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Kazachenko The Great");
MODULE_DESCRIPTION("Registers a device nr. and implement some callback functions");

/* Buffer for data */
static char buffer[255];
static int buffer_pointer = 0;

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "ramzezdriver"
#define DRIVER_CLASS "MyModuleClass"


/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - close was called!\n");
	return 0;
}

/* Global Variable for reading and writing */
int32_t answer = 42;

static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg){
	timer_t timerid;
	struct pid* result_pid;
	struct task_struct* task;
	struct timer_params timer;
	struct mmap_params mmap;
	
	switch(cmd){
		case RD_TIMER:
			if(copy_from_user(&timer, (struct timer_params *) arg, sizeof(struct timer_params))) 
				printk("ioctl_RD_TIMER - Error copying data from user!\n");
			else
				printk("ioctl_RD_TIMER - Received pid: %zu\n", timer.pid);
			
			result_pid = find_get_pid(timer.pid);
			
			/*for_each_process(task) {
				printk("pid: %u  mmap: %x expires: %lu\n", task->pid,  task->mm, task->signal->real_timer.node.expires);
				if(task->pid == timer.pid) {
					break;
				}
			}*/
			
			if(result_pid) {
				task = get_pid_task(result_pid, PIDTYPE_PID);
				
				struct timerqueue_node tqn = task->signal->real_timer.node;
				timer.expires = tqn.expires;
				struct timerqueue_head tqh = task->posix_cputimers.bases[0].tqhead;
				timer.head = &(tqh);
				
				/*struct cpu_timer* cpu_timer;
				timer = container_of(&(timer.head), struct cpu_timer, head));
				printk("real_head: %x timer_head: %x\n", &(tqh), &(cpu_timer->head));
				*/
			
			}
			
			if(copy_to_user((struct timer_params*) arg, &timer, sizeof(struct timer_params))) 
				printk("ioctl_RD_TIMER - Error copying data to user!\n");
			break;
				
					
		case RD_MMAP:
			if(copy_from_user(&mmap, (struct mmap_params *) arg, sizeof(struct mmap_params))) 
				printk("ioctl_RD_MMAP - Error copying data from user!\n");
			else
				printk("ioctl_RD_MMAP - Received pid: %zu\n", mmap.pid);
			
			result_pid = find_get_pid(mmap.pid);
			
			if(result_pid) {
				task = get_pid_task(result_pid, PIDTYPE_PID);
				if (task->mm){
					mmap.vm_start = task->mm->mmap->vm_start;
					mmap.vm_end = task->mm->mmap->vm_end;
					mmap.vm_flags = task->mm->mmap->vm_flags;
				}
							
			}
			
			if(copy_to_user((struct mmap_params*) arg, &mmap, sizeof(struct mmap_params))) 
				printk("ioctl_RD_MMAP - Error copying data to user!\n");
				
			break;	

	}
	return 0;
}


static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = NULL,
	.write = NULL,
	.unlocked_ioctl = my_ioctl
};

/**
 * @brief This function is called, when the module is loaded into the kernel (insmod)
 */
static int __init ModuleInit(void) {
	int retval;
	printk("Hello, Kernel!\n");

	/* Allocate a device nr */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

	/* Create device class */
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created!\n");
		goto ClassError;
	}

	/* create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	return 0;
AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel (rmmod)
 */
static void __exit ModuleExit(void) {
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
