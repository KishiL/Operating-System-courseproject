#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched/signal.h>

static struct task_struct *thread_st;

typedef struct
{
	int from_index;
	int to_index;

}parameters;

parameters *data;

int input[16] = { 4,9,7,37,26,19,4,1,3,5,23,45,27,45,67,89 };
int result[16];

void merger(void *params)
{
	parameters *p = (parameters *)params;
	int i,j,size;
	i = p->from_index;
	j = p->to_index;
	size = sizeof(input)/sizeof(int);
	int position = 0; 
	
	while(i<p->to_index && j<size){
		if(input[i] <= input[j]){
			result[position++] = input[i];
			i++;
		}
		else{
			result[position++] = input[j];
			j++;
		}
	}

	if(i< p->to_index){
		result[position] = input[i];
		position++;
		i++;
	}
	else{
		while(j<size){
		result[position] = input[j];
		position++;
		j++;
		}
	}

}
static int merge_fn(void *Ptr)
{
	//Allow the Sigkill signal
	allow_signal(SIGKILL);
	parameters *data = Ptr;
	int i,num_samples, num_threads;
	num_threads = 2;
	num_samples = 16;

	for(i=0; i<num_threads-1; i++)
	{
		data = (parameters*)vmalloc(sizeof(parameters));
		data->from_index = i*num_samples/num_threads;
		data->to_index = data->from_index + num_samples/num_threads;
		merger(data);
		printk("Merge Loop");
	};

	printk(KERN_INFO "Final Result:\n");
	for(i = 0; i<num_samples; i++)
	{
		printk(KERN_CONT"%d ", result[i]);
	}
	
	do_exit(0);
	return 0;

}

static int thread_fn(void *Ptr)
{
	//Allow the Sigkill signal
	allow_signal(SIGKILL);
	parameters *data = Ptr;
	int i;
	int begin = data->from_index;
	int end = data->to_index;
	int swapped = 1;
	int j = 0;
	int temp;
	printk(KERN_INFO "ORIGINAL ARRAY:");
	for(i=begin;i<=end;i++)
	{
		printk(KERN_CONT "%d ",input[i]);
	}
	while(swapped ==1){
		swapped = 0;
		j++;
		for(i = begin; i<=end-j; i++){
		if(input[i]>input[i+1]){
			temp = input[i];
			input[i]=input[i+1];
			input[i+1]=temp;
			swapped = 1;
			}
		}
	}

	//ssleep(2);
	printk(KERN_INFO "\n SORTED ARRAY:");
	for(i=begin;i<=end;i++)
	{
		printk(KERN_CONT "%d ",input[i]);
	}

	do_exit(0);
	return 0;
}

//Module initialization
static int __init init_thread(void)
{	
	int i, num_threads,num_samples;
	num_threads = 2;
	num_samples = 16;
	for(i=0; i<=num_threads-1; i++){
		data = (parameters*)vmalloc(sizeof(parameters));
		data->from_index = i*num_samples/num_threads;
		data->to_index = data->from_index + num_samples/num_threads -1;
		printk(KERN_INFO "Create Thread#%d\n",i);
		//Create the kernel thread with name 'mythread'
		thread_st=kthread_run(thread_fn, data, "mythread");
		if(thread_st)
		{
			printk("Thread#%d Create Successfully.\n",i);
			wake_up_process(thread_st);
		}
		else
			printk(KERN_ERR "Thread#%d Creation Failed\n",i);
		ssleep(1);
	}

	//Merge Section
	i++;
	data = (parameters*)vmalloc(sizeof(parameters));
	data->from_index = 0;
	data->to_index = 0;
	thread_st = kthread_run(merge_fn, data, "thread#2");
	if (thread_st)
	{
		printk(KERN_INFO "Thread#%d Created Successfully.\n",i);
		wake_up_process(thread_st);
	}
	else
	printk(KERN_ERR "Thread creation failed.\n");
	ssleep(1);
	//Final Result
	//printk(KERN_INFO "Final Result:\n");
	//for(i=0; i<num_samples;i++)
	//{
	//	printk(KERN_CONT "%d ",result[i]);
	//}
	return 0;
}

//Module Exit
static void __exit cleanup_thread(void)
{
	printk("Cleaning Up.\n");
	//if (thread_st)
	//{
	//	kthread_stop(thread_st);
	//	printk(KERN_INFO "Thread Stopped\n");
	//}
}

module_init( init_thread );
module_exit( cleanup_thread );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SIMPLE THREAD EXAMPLE");
MODULE_AUTHOR("YueLi");
