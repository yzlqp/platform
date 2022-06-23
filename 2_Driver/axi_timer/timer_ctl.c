#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>       
#include <linux/gpio.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/cdev.h> 
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h> 
#include <linux/err.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/pm.h>
#include <linux/fs.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/dma-buf.h>
#include <linux/string.h>
#include <linux/dmaengine.h>
#include <linux/completion.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/pagemap.h>
#include <linux/clk.h>
#include <linux/vmalloc.h>
#include <linux/moduleparam.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <linux/notifier.h>
#include <linux/pci.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <dt-bindings/interrupt-controller/irq.h>

//#define DEBUG

#define TIMER_NAME     "axi_timer"                  // name 
#define MEM_SIZE 	    0x1000		                // The maximum global memory is 4KB
#define MEM_CLEAR	    0x1			                // Clear global memory
#define GLOBALMEM_MAJOR	267	                        // Default master device number  
static int globalmem_major = GLOBALMEM_MAJOR;


/*
 * IOCTL custom commands
 */
#define  IOC_MAGIC_TYPE              's'                                // Define IO device type/magic number
#define  IOC_PLRESET                _IO(IOC_MAGIC_TYPE, 0)              // Initialize the system to reset PL
#define  IOC_TX_START_COUNT         _IOW(IOC_MAGIC_TYPE, 1, int)        // Write registers TX_START and TX_COUNT
#define  IOC_RX_COUNT               _IOR(IOC_MAGIC_TYPE, 2, int)        // Read register RX_COUNT
#define  IOC_RX_END                 _IO(IOC_MAGIC_TYPE, 3)              // Notifies the PL end that data has been fetched and can be notified
#define  IOC_TIMER_INTR_EN          _IOW(IOC_MAGIC_TYPE, 4, int)        // Timer (interrupt number can be customized) interrupt enable
#define  IOC_TIMER_INTR_DIS         _IOW(IOC_MAGIC_TYPE, 5, int)        // Timer (interrupt number can be customized) interrupt disable
#define  IOC_MAXNR      


/*
 * The whole idea: Apply a global structure variable as if it were  
 * A FIFO, only when there is data in the FIFO, the read process can  
 * The data is read out, and the data is read out of the FIFO's global memory  
 * The middle was taken out;  The write process can only go to this if the FIFO is not full  
 * Data is written to the FIFO.  
 */
typedef struct gm_cdev {
    struct cdev cdev;					// Character device structure
    struct class *class;                // class
    struct device *device;              // device 
	unsigned char mem[MEM_SIZE];		// global memory
	struct semaphore sem;				// Semaphores for concurrent control
	unsigned int current_len;			// Fifo Valid data length
	wait_queue_head_t r_wait;			// The wait queue head used to block reads
	wait_queue_head_t w_wait;			// Wait queue head for blocking write
    struct device_node *nd;             // device node for dts
    int irq_timer;                      // Interrupt number 0
	struct fasync_struct *async_queue;	// Asynchronous structure pointer
}GLOBALFILO;

GLOBALFILO *globalmem_dev;			    // Device structure pointer

static int globalfifo_fasync(int fd, struct file *filp, int mode);

int  globalmem_open(struct inode *inode, struct file *file)
{
    // Assign the device structure pointer to the file private data pointer
    file->private_data = globalmem_dev;
	return 0;
}

int globalmem_release(struct inode *inode, struct file *file)
{
	// Remove the file from the asynchronous notification list
	globalfifo_fasync(-1, file, 0);
	return 0;
}

/*
 * Timer interrupt handler function
 */
static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
#ifdef DEBUG      
    printk("timer_interrupt = %d\n", irq);
#endif
    GLOBALFILO *dev = (GLOBALFILO *)dev_id;
    kill_fasync (&dev->async_queue, SIGUSR1, POLL_MSG);
	return IRQ_HANDLED;
}


long globalmem_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret;
    unsigned int val;
	GLOBALFILO *devp = file->private_data;

	switch (cmd) {
        // Request timer interrupt handler function
        case IOC_TIMER_INTR_EN:
            ret = copy_from_user(&val, (void *)arg, sizeof(int));
            if (ret) {
                printk(KERN_ERR "IOC_TIMER_INTR_EN: kernel read failed!\n");
                return -EFAULT;
            }
            devp->irq_timer = irq_of_parse_and_map(devp->nd, val);
            ret = request_irq(devp->irq_timer, timer_interrupt, 0, "user_irq_for_timer@0", devp);
            if (ret) {
                printk(KERN_INFO "request irq failed, ret = %d, irq = %d\n", ret, devp->irq_timer);
            }
#ifdef DEBUG
            printk(KERN_INFO "ioctl: IOC_TIMER_INTR_EN, irq = %d, NO.%d\n", devp->irq_timer, val);
#endif
            break;

        // Disable interrupt handler function
        case IOC_TIMER_INTR_DIS:
            free_irq(devp->irq_timer, devp); 
#ifdef DEBUG
            printk(KERN_INFO "ioctl: IOC_TIMER_INTR_DIS\n");
#endif
            break;

		case MEM_CLEAR:
            if (down_interruptible(&devp->sem)) {
                return -ERESTARTSYS;		
			}
			memset(devp->mem, 0, MEM_SIZE);
			up(&devp->sem);
#ifdef DEBUG
			printk(KERN_INFO "globalmem is set to zero\n");
#endif
            break;

        default:
            printk(KERN_ERR "ioctl: insruction default!\n");
            return -EINVAL;  
	}
    return 0;
}


/*static ssize_t globalmem_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	int ret = 0;
	GLOBALFILO *devp = file->private_data;
	DECLARE_WAITQUEUE(wait, current);
    // Concurrency control in semaphores, not spinlocks, Because copy_to_user causes process scheduling, it can cause the kernel collapse  
	if(down_interruptible(&devp->sem)){
		return -ERESTARTSYS;		
	}
	//把wait等待队列挂到等待队列头上
	add_wait_queue(&devp->r_wait, &wait);
	//等待FIFO非空
	while(0 == devp->current_len){
		if(file->f_flags & O_NONBLOCK){
			//如果文件是非阻塞，则直接退出
			ret = -EAGAIN;
			goto out;
		}
		//改变进程状态为睡眠
		__set_current_state(TASK_INTERRUPTIBLE);
		//释放信号量
		up(&devp->sem);
		printk(KERN_INFO "%d,   In globalmem_read, Begin schedule!\n", __LINE__);
		//调用其他进程执行
		schedule();
		printk(KERN_INFO "%d,   In globalmem_read, End schedule!\n", __LINE__);
		//如果信号唤醒此进程
		if(signal_pending(current)){
			ret = -ERESTARTSYS;
			goto out2;
		}
		//获得信号量
		if(down_interruptible(&devp->sem)){
			return -ERESTARTSYS;		
		}

	}
	//把数据从内核空间拷贝到用户空间
	if(size > devp->current_len){
		size = devp->current_len;
	}

	if(copy_to_user(buf, (void *)devp->mem, size)){
		ret = -EFAULT;
		goto out;
	}else{
		//fifo有效数据前移
		memcpy(devp->mem, devp->mem+size, devp->current_len-size);
		//有效数据长度减少
		devp->current_len -= size;

		printk(KERN_INFO "read %u bytes, current_len is %u\n", size, devp->current_len);

		//唤醒写等待队列
		wake_up_interruptible(&devp->w_wait);

		ret = size;
	}

out:
	//释放信号量
	up(&devp->sem);

out2:
	//移除等待队列
	remove_wait_queue(&devp->r_wait, &wait);
	set_current_state(TASK_RUNNING);
	printk(KERN_INFO "%d,   It is end!\n", __LINE__);

	return ret;
}

static ssize_t globalmem_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
	GLOBALFILO *devp = file->private_data;		//从文件结构体中取出设备结构体
	int ret = 0;
	DECLARE_WAITQUEUE(wait, current);	//定义等待队列
	//获得信号量
	if(down_interruptible(&devp->sem)){
		return -ERESTARTSYS;		
	}

	//把wait等待队列挂到等待队列头上
	add_wait_queue(&devp->w_wait, &wait);	

	//等待fifo非满
	while(MEM_SIZE == devp->current_len){
		if(file->f_flags & O_NONBLOCK){
			//如果文件是非阻塞，则直接退出
			ret = -EAGAIN;
			goto out;
		}	

		//改变进程状态为睡眠
		__set_current_state(TASK_INTERRUPTIBLE);

		//释放信号量
		up(&devp->sem);

		printk(KERN_INFO "%d,   In globalmem_write, Begin schedule!\n", __LINE__);
		//调用其他进程执行
		schedule();
		printk(KERN_INFO "%d,   globalmem_write, End schedule!\n", __LINE__);

		//如果信号唤醒此进程
		if(signal_pending(current)){
			ret = -ERESTARTSYS;
			goto out2;
		}

		//获得信号量
		if(down_interruptible(&devp->sem)){
			return -ERESTARTSYS;		
		}

	}
	
	//把数据从用户空间拷贝到内核空间
	if(size > MEM_SIZE - devp->current_len){
		size = MEM_SIZE - devp->current_len;
	}

	if(copy_from_user( (void *)devp->mem+devp->current_len, buf, size)){
		ret = -EFAULT;
		goto out;
	}else{
		devp->current_len += size;

		printk(KERN_INFO "write %u bytes, current len is %u\n", size, devp->current_len);

		//唤醒读等待队列
		wake_up_interruptible(&devp->r_wait);
		
		//产生异步读信号
		if(devp->async_queue){
			printk(KERN_INFO "async_queue is not null!\n");
			kill_fasync(&devp->async_queue, SIGIO, POLL_IN);
		}
		
		ret = size;
	}

out:	
	//释放信号量
	up(&devp->sem);

out2:
	remove_wait_queue(&devp->w_wait, &wait);
	set_current_state(TASK_RUNNING);
	printk(KERN_INFO "%d,   It is end!\n", __LINE__);

	return ret;
}*/


static int globalfifo_fasync(int fd, struct file *filp, int mode)
{
	GLOBALFILO *devp = filp->private_data;
	return fasync_helper(fd, filp, mode, &devp->async_queue);
}

static const struct file_operations global_fops = {
    .owner = THIS_MODULE,	
	.llseek = NULL,
	.unlocked_ioctl = globalmem_ioctl,
	.fasync = globalfifo_fasync,
	.compat_ioctl = NULL,
	.open = globalmem_open,
	.release = globalmem_release,
};

static void globalmem_setup_cdev(GLOBALFILO *cdev, int index)
{
	int errno;
	int devno = MKDEV(globalmem_major, index);	
	cdev_init(&cdev->cdev, &global_fops);		
	cdev->cdev.owner = THIS_MODULE;
	errno = cdev_add(&cdev->cdev, devno, 1);
	if(errno)
		printk(KERN_NOTICE "Add %d dev is error %d", index, errno);
}

int globalmem_init(void)
{
	int result, ret;
	dev_t devno = MKDEV(globalmem_major, 0);
	if (globalmem_major) {
		result = register_chrdev_region(devno, 1, "globalmem");
	} else {
		result = alloc_chrdev_region(&devno, 0 , 1, "globalmem");
		globalmem_major = MAJOR(devno);
	}
	if (result < 0) {
		return result;
	}

	globalmem_dev = kmalloc(sizeof(GLOBALFILO), GFP_KERNEL);
	if (!globalmem_dev) {
		result = -ENOMEM;
		goto err;
	}

	memset(globalmem_dev, 0, sizeof(GLOBALFILO));
	globalmem_setup_cdev(globalmem_dev, 0);
	sema_init(&globalmem_dev->sem, 1);
	init_waitqueue_head(&globalmem_dev->r_wait);
	init_waitqueue_head(&globalmem_dev->w_wait);

    // Get device tree Axidev node (not required)
    globalmem_dev->nd = of_find_node_by_path("/axidev@0");
    if (NULL == globalmem_dev->nd) {
        printk(KERN_ERR "axidev@0: Failed to get node\n");
        return -EINVAL;
    }

    // Creat a class
    globalmem_dev->class = class_create(THIS_MODULE, TIMER_NAME);
    if (IS_ERR(globalmem_dev->class)) {
        ret = PTR_ERR(globalmem_dev->class);
        goto err;
    }

    // Create device device to automatically generate device file /dev/timer_name
    globalmem_dev->device = device_create(globalmem_dev->class, NULL, devno, NULL, TIMER_NAME);
    if (IS_ERR(globalmem_dev->device)) {
        ret = PTR_ERR(globalmem_dev->device);
        goto err;
    }

#ifdef DEBUG
    printk(KERN_INFO "globalmem_init success\n");
#endif
	return 0;

err:
	unregister_chrdev_region(devno, 1);
	return result;
}

void globalmem_exit(void)
{
	cdev_del(&globalmem_dev->cdev);	
	kfree(globalmem_dev);
	unregister_chrdev_region(MKDEV(globalmem_major, 0), 1);
}

MODULE_LICENSE("Dual BSD/GPL");
module_param(globalmem_major, int, S_IRUGO);
module_init(globalmem_init);
module_exit(globalmem_exit);

