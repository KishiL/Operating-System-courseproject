#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>

static struct task_struct *thread_st;

static int thread_fn(void *unused)
{
	while(1)
	{
		printk(KERN_INFO "Thread Running\n");
		ssleep(5);
	}

	printk(KERN_INFO "Thread Stopping\n");
	do_exit(0);
	return 0;
}

// Module Initialization
static int __init init_thread(void)
{
	printk(KERN_INFO "Creating Thread \n");
	//Create the kernel thread with name 'mythread'
	thread_st = kthread_create(thread_fn,NULL, "mythread");
	if(thread_st)
		printk("Thread Created successfully\n");
	else
		printk(KERN_INFO "Thread creation failed\n");
	return 0;
}

//Module Exit
static void __exit cleanup_thread(void)
{
	printk("Cleaning Up\n");
}