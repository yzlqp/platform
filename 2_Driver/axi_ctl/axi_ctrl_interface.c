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

#define DEBUG


/*
 * Device information
 */
#define AXIDEV_NAME     "axi_ctrl_cdev"               // name 
#define AXIDEV_CNT      1                             // The number of devices 
#define AXIDEV_MAJOR    266                           // Default primary device number 
static int axidev_major = AXIDEV_MAJOR;
module_param(axidev_major, int, S_IRUGO);

/* 
 * Defining key state 
 */
enum key_status {
    KEY_PRESS = 0,                                   // Button press
    KEY_RELEASE,                                     // Button release
    KEY_KEEP_PRESS,                                  // The button state is held down
    KEY_KEEP_RELEASE                                 // The button state remains loose
};
static int keystatus = KEY_KEEP_RELEASE;             // Default key state

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
#define  IOC_MAXNR                  5                                   // IOC numbers

/*
 * Relevant register address definition (deprecated)
 */
/*#define  AXI_CTRL_INTERFACE_BASEADDR  0x43C00000              // start addr
#define  AXI_CTRL_INTERFACE_HIGHADDR  0x43C0FFFF                // end addr 64K
#define  AXI_CTRL_BRAM_TX_START_0         0x43C00000            // ((volatile void *) 0x43C00000)   //reg0   bit[0]       w
#define  AXI_CTRL_TX_COUNT_0		      0x43C00004            // ((volatile void *) 0x43C00004)   //reg1   bit[15:0]    w
#define	 AXI_CTRL_MODE          	      0x43C00008            // ((volatile void *) 0x43C00008)   //reg2   bit[31:0]    w
#define	 AXI_CTRL_BRAM_RX_READY_0		  0x43C0000C            // ((volatile void *) 0x43C0000C)   //reg3   bit[0]       w
#define	 AXI_CTRL_BRAM_RX_READY_1		  0x43C00010            // ((volatile void *) 0x43C00010)   //reg4   bit[0]       W
#define	 AXI_CTRL_TX_READY_0		      0x43C00014            // ((volatile void *) 0x43C00014)   //reg5   bit[0]       r
#define  AXI_CTRL_BRAM_TX_START_1         0x43C00018            // ((volatile void *) 0x43C00018)   //reg6   bit[0]       w
#define  AXI_CTRL_TX_COUNT_1		      0x43C0001C            // ((volatile void *) 0x43C0001C)   //reg7   bit[15:0]    w
#define	 AXI_CTRL_TX_READY_1		      0x43C00020            // ((volatile void *) 0x43C00020)   //reg8   bit[0]       r
#define	 AXI_CTRL_RX_COUNT_0		      0x43C00024            // ((volatile void *) 0x43C00024)   //reg9   bit[15:0]    r
#define	 AXI_CTRL_RX_COUNT_1		      0x43C00028            // ((volatile void *) 0x43C00028)   //reg10  bit[15:0]    r
#define	 AXI_CTRL_SNR		              0x43C0002C            // ((volatile void *) 0x43C0002C)   //reg11  bit[15:0]    r*/

/*
 * Virtual address pointer to a mapped register
 */
static volatile void __iomem *bram_tx_start;
static volatile void __iomem *tx_count;
static volatile void __iomem *reserved0;
static volatile void __iomem *bram_rx_end;
static volatile void __iomem *bram_tx_end;
static volatile void __iomem *rx_count;
static volatile void __iomem *reserved1;
static volatile void __iomem *reserved2;

/*
 * Device structure
 */
struct axi_ctrl_interface_dev {
    dev_t devid;                        // device number 
    struct cdev cdev;                   // cdev Character device structure
    struct class *class;                // class
    struct device *device;              // device 
    struct device_node *nd;             // device node for dts
    int irq_num_0;                      // Interrupt number 0
    int irq_num_1;                      // Interrupt number 1
    int irq_timer;                      // Interrupt number 2
    int gpio_res_0;                     // GPIO reserved 0 for reset PL
    int gpio_res_1;                     // GPIO reserved 1 
    int gpio_key;                       // GPIO key 
    spinlock_t spinlock;                // spinlock 
    struct mutex mutex;                 // mutex 
    struct timer_list timer;            // timer 
    atomic_t    counter;                // Atomic variable
    wait_queue_head_t r_wait;           // Read wait queue headers
    struct fasync_struct *async_queue;  // fasync_struct structure 
};

/*
 * Device pointer
 */
static struct axi_ctrl_interface_dev *axidevp;



/*
 * @param -- inode: the inode passed to the driver
 * @param -- filp: The device file, file structure has a member variable called private_data which normally points private_data to the device structure when opened.
 * @return: 0 succeeded; Other failure
 *
 */
static int axidev_open(struct inode *inode, struct file *filp)
{
    filp->private_data = axidevp;
    return 0;
}

/*
 * @param -- filp: Device file to open (file descriptor)
 * @param -- buf: data buffer returned to user space
 * @param -- CNT: The length of data to read
 * @param -- offt: offset from the start of the file
 * @return: Number of bytes read. If the value is negative, the read fails
 *
 */
static ssize_t axidev_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    //unsigned long p = *offt;
    //unsigned int count = cnt;
    //unsigned long flags;
    int ret = 4;
    //char buffer[];
    unsigned int val;

    //struct axi_ctrl_interface_dev *devp = (struct axi_ctrl_interface_dev *)filp->private_data;
    //spin_lock_irqsave(&devp->spinlock, flags);
    
    val = readl(rx_count);
    if(copy_to_user(buf, &val, cnt)){
        printk(KERN_ERR "axidev_read: kernel read failed!\n");
        ret = -EFAULT;
    }

    //spin_unlock_irqrestore(&devp->spinlock, flags);
    
    return ret;
}

/*
 * @param -- filp: device file, representing the open file descriptor
 * @param -- buf: data to be written to the device
 * @param -- CNT: The length of data to write
 * @param -- offt: offset from the start of the file
 * @return: Number of bytes written. If the value is negative, the write fails
 *
 */
static ssize_t axidev_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
    int ret = 4;
    unsigned int val;

    if(copy_from_user(&val, buf, cnt)) {
        printk(KERN_ERR "axidev_write: kernel write failed!\n");
        ret = -EFAULT;
    }

    //val = *((unsigned int *)buffer)
    writel(val, tx_count);
    return ret;
}

/*
 * Timer interrupt handler function
 */
static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
#ifdef DEBUG      
    printk("timer_interrupt = %d\n", irq);
#endif
    struct axi_ctrl_interface_dev *dev = (struct axi_ctrl_interface_dev *)dev_id;
    kill_fasync (&dev->async_queue, SIGIO, POLL_MSG);
	return IRQ_HANDLED;
}

/*
 * @param -- filp: device file, representing the open file descriptor
 * @param -- cmd: command
 * @param -- arg: arguments
 *
 */
static long axidev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret;
    unsigned int val;

    struct axi_ctrl_interface_dev *devp = (struct axi_ctrl_interface_dev *)filp->private_data;

    // Check the device typ, this choice between robustness and processing speed
    /*if (_IOC_TYPE(cmd) != IOC_MAGIC_TYPE) {
        pr_err("[%s] command type [%c] error!\n", __func__, _IOC_TYPE(cmd));
        return -ENOTTY; 
    }

    // Check the ordinal numbe
    if (_IOC_NR(cmd) > IOC_MAXNR) { 
        pr_err("[%s] command numer [%d] exceeded!\n", __func__, _IOC_NR(cmd));
        return -ENOTTY;
    } 

    // Check the access mode
    if (_IOC_DIR(cmd) & _IOC_READ)
        ret= !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        ret= !access_ok(VERIFY_READ,  (void __user *)arg, _IOC_SIZE(cmd));
    if (ret)
        return -EFAULT;*/

    switch (cmd) {

        // Write registers TX START and TX COUNT 
        case IOC_TX_START_COUNT:
            ret = copy_from_user(&val, (void *)arg, sizeof(int));
            if (ret) {
                printk(KERN_ERR "IOC_TX_START_COUNT: kernel read failed!\n");
                return -EFAULT;
            }
            writel(val, tx_count);     
            writel(0, bram_tx_start);      // 50 - 60  clocks 
            writel(1, bram_tx_start);
            writel(0, bram_tx_start);
#ifdef DEBUG
            printk(KERN_INFO "axidev_ioctl: IOC_TX_START_COUNT, count = %d\n", val);
#endif
            break;

        // Read Rx count register
        case IOC_RX_COUNT:
            val = readl(rx_count);
            ret = copy_to_user((void *)arg, &val, sizeof(int));
            if (ret) {
                printk(KERN_ERR "IOC_RX_COUNT: kernel read failed!\n");
                return -EFAULT;
            }
#ifdef DEBUG           
            printk(KERN_INFO "axidev_ioctl: IOC_RX_COUNT, count = %d\n", val);
#endif          
            break;

        // Notify the PL data read over
        case IOC_RX_END:
            writel(0, bram_rx_end);
            writel(1, bram_rx_end);
            writel(0, bram_rx_end);
#ifdef DEBUG              
            printk(KERN_INFO "axidev_ioctl: IOC_RX_END\n");
#endif             
            break;
        
        // Initializes the device to reset PL
        case IOC_PLRESET:  
            gpio_set_value(devp->gpio_res_0, 0);
            gpio_set_value(devp->gpio_res_0, 1);
            gpio_set_value(devp->gpio_res_0, 0);
#ifdef DEBUG
            printk(KERN_INFO "axidev_ioctl: IOC_PLRESET\n");
#endif
            break;

        // Request interrupt handler function
        case IOC_TIMER_INTR_EN:
            ret = copy_from_user(&val, (void *)arg, sizeof(int));
            if (ret) {
                printk(KERN_ERR "IOC_TIMER_INTR_EN: kernel read failed!\n");
                return -EFAULT;
            }
            devp->irq_timer = irq_of_parse_and_map(devp->nd, 2);
            ret = request_irq(devp->irq_timer, timer_interrupt, 0, "user_irq@0", axidevp);
            if (ret) {
                printk(KERN_INFO "request irq failed, ret = %d, irq = %d\n", ret, devp->irq_timer);
            }
#ifdef DEBUG
            printk(KERN_INFO "axidev_ioctl: IOC_TIMER_INTR_EN, irq = %d\n", devp->irq_timer);
#endif
            break;

        // Disable interrupt handler function
        case IOC_TIMER_INTR_DIS:
            ret = copy_from_user(&val, (void *)arg, sizeof(int));
            if (ret) {
                printk(KERN_ERR "IOC_TIMER_INTR_DIS: kernel read failed!\n");
                return -EFAULT;
            }
            free_irq(devp->irq_timer, axidevp); 
#ifdef DEBUG
            printk(KERN_INFO "axidev_ioctl: IOC_TIMER_INTR_DIS\n");
#endif
            break;

        default:
            printk(KERN_ERR "axidev_ioctl: insruction default!\n");
            return -EINVAL;  
    }

    return 0;
}

/*
 *  Timer function
 */
static void key_timer_function(unsigned long arg)
{
    static int last_val = 1;
    unsigned long flags;
    int current_val;

    spin_lock_irqsave(&axidevp->spinlock, flags);

    // Read the key value and determine the current state of the key 
    current_val = gpio_get_value(axidevp->gpio_key);
    if ((0 == current_val) && last_val) { 
        keystatus = KEY_PRESS;                      // Press process
    } else if ((1 == current_val) && !last_val){
        keystatus = KEY_RELEASE;                    // Loosen the process
    } else if ((0 == current_val) && !last_val){
        keystatus = KEY_KEEP_PRESS;                 // State hold down
    } else {
        keystatus = KEY_KEEP_RELEASE;               // State hold loose
    }
 
    last_val = current_val;
    spin_unlock_irqrestore(&axidevp->spinlock, flags);
}

/*
 * Key handler function
 */
static irqreturn_t key_interrupt(int irq, void *dev_id)
{
    // Button anti-shake processing, enable timer delay 15ms
    mod_timer(&axidevp->timer, jiffies + msecs_to_jiffies(15));
    return IRQ_HANDLED;
}

/*
 * Gets the device resource function for platform device
 */
/*static int axidev_get_platform_resource(struct platform_device *dev)
{
    int i;
    struct resource *res[8];
    
    //Obtain resources
    for (i = 0; i < 8; i++) {
        res[i] = platform_get_resource(dev, IORESOURCE_MEM, i);
        if (!res[i]) {
            printk(KERN_ERR "no MEM resource found %d\n", i);
            return -ENXIO;
        }
    }
    //Register address mapping, which maps physical addresses to virtual addresses
    bram_tx_start = ioremap(res[0]->start, resource_size(res[0]));
    tx_count = ioremap(res[1]->start, resource_size(res[1]));
    reserved0 = ioremap(res[2]->start, resource_size(res[2]));
    bram_rx_end = ioremap(res[3]->start, resource_size(res[3]));
    bram_tx_end = ioremap(res[4]->start, resource_size(res[4]));
    rx_count = ioremap(res[5]->start, resource_size(res[5]));
    reserved1 = ioremap(res[6]->start, resource_size(res[6])); 
    reserved2 = ioremap(res[7]->start, resource_size(res[7])); 
    
    return 0;
}*/

/*  
 * @description: the fasync function, which handles asynchronous notifications  
 * @param -- fd: file descriptor  
 * @param -- filp: Device file to open (file descriptor)  
 * @param -- on: mode  
 * @return: A negative number indicates that the function failed
 *  
 */  
static int axidev_fasync(int fd, struct file *filp, int on)
{
    struct axi_ctrl_interface_dev *dev = filp->private_data;
    return fasync_helper(fd, filp, on, &dev->async_queue);
}

/*
 * Device release function
 */
int axidev_release (struct inode *inode, struct file *filp)
{
    axidev_fasync(-1, filp, 0);
    return 0;
}

/*
 * Character device driver operation set
 */
static struct file_operations axidev_fops = {
    .owner = THIS_MODULE,
    .open = axidev_open,
    .read = axidev_read,
    .write = axidev_write,
    .release = axidev_release,
    .unlocked_ioctl = axidev_ioctl,
    .fasync = axidev_fasync,
};

/*
 * Receive data interrupt handler function
 */
static irqreturn_t irq_interrupt(int irq, void *dev_id)
{
#ifdef DEBUG      
    printk("irq_interrupt = %d\n", irq);
#endif
    struct axi_ctrl_interface_dev *dev = (struct axi_ctrl_interface_dev *)dev_id;
    kill_fasync (&dev->async_queue, SIGIO, POLL_IN);

	return IRQ_HANDLED;
}

/*
 * Reserved interrupt handler function
 */
static irqreturn_t irq_interrupt_1(int irq, void *dev_id)
{
    printk("irq = %d\n", irq);
	return IRQ_HANDLED;
}

/*
 * Device tree parsing function
 */
static int axidev_parse_dt(struct device_node *nd)
{
    const char *str;
    int ret;

    // Get device tree Axidev node (not required)
    //nd = of_find_node_by_path("/axidev@0");
    //if(NULL == nd) {
    //    printk(KERN_ERR "axidev@0: Failed to get node\n");
    //    return -EINVAL;
    //}

    // Read the Status property
    ret = of_property_read_string(nd, "status", &str);

#ifdef DEBUG
    printk(KERN_INFO "axidev_parse_dt: status = %s\n",*str);
#endif

    if(!ret) {
        if (strcmp(str, "okay"))
        return -EINVAL;
    }

    // Get the compatible property value and match it
    ret = of_property_read_string(nd, "compatible", &str);
    if(ret)
        return ret;

#ifdef DEBUG
    printk(KERN_INFO "axidev_parse_dt: compatible = %s\n",*str);
#endif

    if (strcmp(str, "axi_ctrl_interface")) {
        printk(KERN_ERR "axidev@0: Compatible match failed\n");
        return -EINVAL;
    }

    // Obtain the reserved-GPIO attribute in the device tree to obtain the GPIO number of the key  
    axidevp->gpio_res_0 = of_get_named_gpio(nd, "reserved-gpio", 0);
    if(!gpio_is_valid(axidevp->gpio_res_0)) {
        printk(KERN_ERR "axidev@0: Failed to get reserved-gpio-0\n");
        return -EINVAL;
    }

    axidevp->gpio_res_1 = of_get_named_gpio(nd, "reserved-gpio", 1);
    if(!gpio_is_valid(axidevp->gpio_res_1)) {
        printk(KERN_ERR "axidev@0: Failed to get reserved-gpio-1\n");
        return -EINVAL;
    }

    axidevp->gpio_key = of_get_named_gpio(nd, "key-gpio", 0);
    if(!gpio_is_valid(axidevp->gpio_key)) {
        printk(KERN_ERR "axidev@0: Failed to get gpio_key\n");
        return -EINVAL;
    }

#ifdef DEBUG
    printk(KERN_INFO "axidev_parse_dt: gpio0 = %d, gpio1 = %d, key = %d\n",axidevp->gpio_res_0, axidevp->gpio_res_1, axidevp->gpio_key);
#endif

    // Gets the corresponding interrupt number
    axidevp->irq_num_0 = irq_of_parse_and_map(nd, 0);
    if (!axidevp->irq_num_0)
        return -EINVAL;
    
    axidevp->irq_num_1 = irq_of_parse_and_map(nd, 1);
    if (!axidevp->irq_num_1)
        return -EINVAL;

#ifdef DEBUG
    printk(KERN_INFO "axidev_parse_dt: irq_num_0 = %d, irq_num_1 = %d\n", axidevp->irq_num_0, axidevp->irq_num_1);
#endif
    
    axidevp->nd = nd;

    return 0;
}

/*
 * Address mapping function
 */
static inline void axidev_of_iomap(void)
{
    bram_tx_start = of_iomap(axidevp->nd, 0);
    tx_count = of_iomap(axidevp->nd, 1);
    reserved0 = of_iomap(axidevp->nd, 2);
    bram_rx_end = of_iomap(axidevp->nd, 3);
    bram_tx_end = of_iomap(axidevp->nd, 4);
    rx_count = of_iomap(axidevp->nd, 5);
    reserved1 = of_iomap(axidevp->nd, 6);
    reserved2 = of_iomap(axidevp->nd, 7);
}

/*
 * Address mapping cancellation function
 */
static inline void axidev_iounmap(void)
{
    // Register address unmapped *
    iounmap(bram_tx_start);
    iounmap(tx_count);
    iounmap(reserved0);
    iounmap(bram_rx_end);
    iounmap(bram_tx_end);
    iounmap(rx_count);
    iounmap(reserved1);
    iounmap(reserved2);
}

/*
 * Initialization and addition of cdev
 */
static int axidev_setup_cdev(struct axi_ctrl_interface_dev *dev, dev_t devno)
{
    int ret = 0;
    dev->devid = devno;
    cdev_init(&dev->cdev, &axidev_fops);             // Initializes the character device driver
    dev->cdev.owner = THIS_MODULE;  
    ret = cdev_add(&dev->cdev, devno, AXIDEV_CNT);   // Add a cdev
    if (ret) {
        printk(KERN_NOTICE "Error -> axidev_setup_cdev: %d, %d\n", ret, devno); 
    } 
    return ret;
}

/*  
 * @description: indicates the probe function of the platform driver when the platform driver is connected to the platform device This function is executed after the match  
 * @param -- dev: platform device pointer  
 * @return: 0, success;  Other negative values, fail  
 *
 */  
static int axidev_probe(struct platform_device *pdev)
{
    int ret;

#ifdef DEBUG
    printk(KERN_INFO "axidev_probe: 1 - The driver and device has been matched\n");
#endif

    // Application for device numbe
    dev_t devno; 
    int axidev_minor = 0;                            
    if (axidev_major) {
        devno = MKDEV(axidev_major, axidev_minor);                               // The primary device number is defined     
        ret = register_chrdev_region(devno, AXIDEV_CNT, AXIDEV_NAME);            // Apply to the system equipment
    } else {                                                                     // No device number is defined
        ret = alloc_chrdev_region(&devno, 0, AXIDEV_CNT, AXIDEV_NAME);
        axidev_major = MAJOR(devno);                                             // Gets the assigned primary and secondary device numbers
        axidev_minor = MINOR(devno);
    }
    if (ret < 0)                                                                 // When the return value is less than zero, said registration failure
        return ret;
#ifdef DEBUG
    printk(KERN_INFO "axidev_probe: 2 - devname = %s, major = %d, minor = %d\n", AXIDEV_NAME, axidev_major, axidev_minor);
#endif

    // Apply a copy of the axi_CTRl_interface_dev structure memory and clear 0
    axidevp = kzalloc(sizeof(struct axi_ctrl_interface_dev), GFP_KERNEL);   
    if (!axidevp) {
        ret = -ENOMEM;
        goto out1;
    }
#ifdef DEBUG
    printk(KERN_INFO "axidev_probe: 3 - kzalloc size = %d Byte\n",sizeof(struct axi_ctrl_interface_dev));
#endif

    //ret = axidev_get_platform_resource(dev);
    // Obtain platform device resources
    spin_lock_init(&axidevp->spinlock);
    mutex_init(&axidevp->mutex);

    // Initialize the mutex 
    ret =axidev_parse_dt(pdev->dev.of_node);
    if (ret) {
        goto outa;
    }       
#ifdef DEBUG
    printk(KERN_INFO "axidev_probe: 4 - axidev_parse_dt success\n");
#endif

    // Apply to GPIO subsystem to use GPIO
    ret = gpio_request(axidevp->gpio_res_0, "axi-gpio-0");
    if (ret) {
        printk(KERN_ERR "axi-gpio: Failed to request axi-gpio-0\n");
        goto outa;
    }
    ret = gpio_request(axidevp->gpio_res_1, "axi-gpio-1");
    if (ret) {
        printk(KERN_ERR "axi-gpio: Failed to request axi-gpio-1\n"); 
        goto outa;
    }
    ret = gpio_request(axidevp->gpio_key, "axi-key");
    if (ret) {
        printk(KERN_ERR "axi-gpio: Failed to request axi-key\n");
        goto outa;
    }
    gpio_direction_output(axidevp->gpio_res_0, 0);   // Reset the PL
    gpio_direction_output(axidevp->gpio_res_1, 0);
    gpio_direction_input(axidevp->gpio_key);
#ifdef DEBUG
    printk(KERN_INFO "axidev_probe: 5 - gpio_request success %d, %d, %d\n", axidevp->gpio_res_0, axidevp->gpio_res_1, axidevp->gpio_key);
#endif

    // Set gPIO pin to output input mode
    unsigned long irq_flags_0 = irq_get_trigger_type(axidevp->irq_num_0);
    /*if (IRQF_TRIGGER_NONE == irq_flags_0)
        irq_flags_0 = IRQF_TRIGGER_RISING;*/
    unsigned long irq_flags_1 = irq_get_trigger_type(axidevp->irq_num_1);
    /*if (IRQF_TRIGGER_NONE == irq_flags_1)
        irq_flags_1 = IRQF_TRIGGER_RISING;*/
#ifdef DEBUG
    printk(KERN_INFO "axidev_probe: 6 - irq_flags_0 = %ld, irq_flags_1 = %ld\n", irq_flags_0, irq_flags_1);
#endif

    // Request interrupt 
    ret = request_irq(axidevp->irq_num_0, irq_interrupt, 0, "axi_ctrl_irq@0", axidevp);
    if (ret) {
        printk(KERN_INFO "axidev_probe: error !!! request irq failed, ret = %d, irq = %d\n", ret, axidevp->irq_num_0);
        goto outb;
    }
#ifdef DEBUG
    printk(KERN_INFO "axidev_probe: 7 - request irq %d success\n", axidevp->irq_num_0);
#endif
    ret = request_irq(axidevp->irq_num_1, irq_interrupt_1, 0, "axi_ctrl_irq@1", axidevp);
    if (ret) {
        printk(KERN_INFO "axidev_probe: error !!! request irq failed, ret = %d, irq = %d\n", ret, axidevp->irq_num_1);
        goto outc;
    }	
#ifdef DEBUG
    printk(KERN_INFO "axidev_probe: 8 - request irq %d success\n", axidevp->irq_num_1);
#endif 
 
    // Register address mapping
    axidev_of_iomap();
#ifdef DEBUG
    printk(KERN_INFO "axidev_probe: 9 - of_iomap success\n");
#endif
    
    // Initialize and apply for a character device driver
    ret = axidev_setup_cdev(axidevp, devno);
    if(ret) {
       goto out2; 
    }

    // Creat a class
    axidevp->class = class_create(THIS_MODULE, AXIDEV_NAME);
    if (IS_ERR(axidevp->class)) {
        ret = PTR_ERR(axidevp->class);
        goto out3;
    }

    // Create device device to automatically generate device file /dev/axidev_name
    axidevp->device = device_create(axidevp->class, NULL, devno, NULL, AXIDEV_NAME);
    if (IS_ERR(axidevp->device)) {
        ret = PTR_ERR(axidevp->device);
        goto out4;
    }
#ifdef DEBUG
    printk(KERN_INFO "axidev_probe: 10 - cdev_init -> cdev_add -> class_create -> device_create success\n");
#endif 

    return 0;

out4:
    class_destroy(axidevp->class);
out3:
    cdev_del(&axidevp->cdev);
out2:
    axidev_iounmap();
    free_irq(axidevp->irq_num_1, NULL); 
outc:
    free_irq(axidevp->irq_num_0, NULL); 
outb:
    gpio_free(axidevp->gpio_res_0);
    gpio_free(axidevp->gpio_res_1);
    gpio_free(axidevp->gpio_key);
outa:
    kfree(axidevp);
out1:
    unregister_chrdev_region(devno, AXIDEV_CNT);
    
    return ret;
}

/*  
 * @description: This function is executed when the platform driver module is uninstalled  
 * @param -- dev: platform device pointer  
 * @return: 0, success;  Other negative values, fail  
 */  
static int axidev_remove(struct platform_device *dev)
{
    printk(KERN_INFO "axidev_remove: platform driver remove!\n");

    // Unregister the device
    device_destroy(axidevp->class, axidevp->devid);
    // Cancellation of class
    class_destroy(axidevp->class);
    // Delete the cdev
    cdev_del(&axidevp->cdev);
    // Deregister the device id
    unregister_chrdev_region(axidevp->devid, AXIDEV_CNT);
    // Deleting an Address Mapping
    axidev_iounmap();
    // Release the GPIO
    gpio_free(axidevp->gpio_res_0);
    gpio_free(axidevp->gpio_res_1);
    gpio_free(axidevp->gpio_key);
    // Release the interrupt
    free_irq(axidevp->irq_num_0, axidevp); 
    free_irq(axidevp->irq_num_1, axidevp);
    // Release memory
    kfree(axidevp);

    return 0;
}

/*
 * Power management-related operations
 */
static int axidev_suspend(struct device *dev)
{
	return 0;
} 
static int axidev_resume(struct device *dev)
{
	return 0;
}
static const struct dev_pm_ops axidev_pm_ops = {
	.suspend = axidev_suspend,
	.resume  = axidev_resume,
};

/*
 * Match list
 */
static const struct of_device_id axidev_of_match[] = {
    { .compatible = "axi_ctrl_interface" },
    { /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, axidev_of_match);

/*
 * Platform Indicates the platform driver architecture
 */
static struct platform_driver axidev_driver = {
    .driver = {
        .owner          = THIS_MODULE,
        .name           = "axi_ctrl_drv",       // Driver name, used to match the device
        .pm    			= &axidev_pm_ops,
        .of_match_table = axidev_of_match,
        },
        .probe = axidev_probe,                 
        .remove = axidev_remove,            
};

/*static int __init axidev_driver_init(void)
{
    return platform_driver_register(&axidev_driver);
}
static void __exit axidev_driver_exit(void)
{
    platform_driver_unregister(&axidev_driver);
}
module_init(axidev_driver_init);
module_exit(axidev_driver_exit);*/

module_platform_driver(axidev_driver);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ylp");
