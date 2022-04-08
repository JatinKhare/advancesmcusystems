/* ==================================:q
 * =================================
 *  cdma-interrupt.c
 *
 *  AUTHOR:     Mark McDermott
 *  CREATED:    March 12, 2009
 *  UPDATED:    May 2, 2017     Updated for ZED Board
 *  UPDATED:    Feb 5, 2021     Updated for the ULTRA96
 *
 *  DESCRIPTION: This kernel module registers interrupts from GPIO
 *               port and measures the time between them. This is
 *               used to also measure the latency through the kernel
 *               to respond to interrupts. 
 *               
 *  DEPENDENCIES: Works on the AVNET Ultra96 Board
 *
 */
 

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/gpio/driver.h>

#include <linux/pm_runtime.h>
#include <linux/of.h>

#define MODULE_VER "1.0"

#define GPIO_MAJOR 241                  // Need to mknod /dev/cdma_int c 241 0
#define MODULE_NM "cdma_interrupt"

#undef DEBUG
#define DEBUG

int             interruptcount  = 0; //tells us how many interrupts we got, the count can be seen in 'more /proc/interrupts'
int             temp            = 0; 
int             len             = 0;
char            *msg            = NULL;
unsigned int    gic_interrupt;

static struct fasync_struct *fasync_cdma_queue ;

/* ===================================================================
 * function: cdma_int_handler
 *
 * This function is the cdma_interrupt handler. It sets the tv2
 * structure using do_gettimeofday.
 */
 
//static irqreturn_t cdma_int_handler(int irq, void *dev_id, struct pt_regs *regs)

irq_handler_t cdma_int_handler(int irq, void *dev_id, struct pt_regs *regs)
{
  interruptcount++;
  
    #ifdef DEBUG
    printk(KERN_INFO "cdma_int: Interrupt detected in kernel \n");  // DEBUG
    #endif
  
    /* Signal the user application that an interupt occured */
    /* SIGIO goes to the application*/
    // interrupt number 51: cdma interrupt
    // interrupt number 48: interrupt_out 
  
    kill_fasync(&fasync_cdma_queue, SIGIO, POLL_IN);

return  (irq_handler_t) IRQ_HANDLED;

}


static struct proc_dir_entry *proc_cdma_int;

/* ===================================================================
*    function: read_proc   --- Example code
*/


ssize_t read_proc(struct file *filp,char *buf,size_t count,loff_t *offp ) 
{

    if(count>temp)
    {
        count=temp;
    }

    temp=temp-count;
    //copy_to_user(buf,msg, count);
    
    if(count==0) temp=len;
    printk("read_proc count value = %ld\n", count);
      
return count;
}


/* ===================================================================
*    function: write_proc   --- Example code
*/

ssize_t write_proc(struct file *filp,const char *buf,size_t count,loff_t *offp)
{
    //copy_from_user(msg,buf,count);
    len=count;
    temp=len;

    printk("write_proc count value = %ld\n", count);

return count; //sends to the kernel module
}


/* ===================================================================
 * function: cdma_open
 *
 * This function is called when the cdma_int device is opened
 *
 */
 
static int cdma_open (struct inode *inode, struct file *file) {

#ifdef DEBUG
        printk(KERN_INFO "cdma_int: Inside cdma_open \n");  // DEBUG
#endif
    return 0;
}

/* ===================================================================
 * function: cdma_release
 *
 * This function is called when the cdma_int device is
 * released
 * 
 */
 
static int cdma_release (struct inode *inode, struct file *file) {

#ifdef DEBUG
        printk(KERN_INFO "\ncdma_int: Inside cdma_release \n");  // DEBUG
#endif
    return 0;
}

/* ===================================================================
 * function: cdma_fasync
 *
 * This is invoked by the kernel when the user program opens this
 * input device and issues fcntl(F_SETFL) on the associated file
 * descriptor. fasync_helper() ensures that if the driver issues a
 * kill_fasync(), a SIGIO is dispatched to the owning application.
 */

static int cdma_fasync (int fd, struct file *filp, int on)
{
    #ifdef DEBUG
    printk(KERN_INFO "\ncdma_int: Inside cdma_fasync \n");  // DEBUG
    #endif
    
    return fasync_helper(fd, filp, on, &fasync_cdma_queue);
}; 

/* ===================================================================
*
*  Define which file operations are supported
*
*/

struct file_operations cdma_fops = {
    .owner          =    THIS_MODULE,
    .llseek         =    NULL,
    .read           =    NULL,
    .write          =    NULL,
    .poll           =    NULL,
    .unlocked_ioctl =    NULL,
    .mmap           =    NULL,
    .open           =    cdma_open,
    .flush          =    NULL,
    .release        =    cdma_release,
    .fsync          =    NULL,
    .fasync         =    cdma_fasync,
    .lock           =    NULL,
    .read           =    NULL,
    .write          =    NULL,
};

struct file_operations proc_fops = {
    read: read_proc, //'more'
    write: write_proc //'echo'
};


/* number 51
dma@b0000000 {
			#dma-cells = <0x1>;
			clock-names = "m_axi_aclk", "s_axi_lite_aclk";
			clocks = <0x3 0x47 0x3 0x47>;
			compatible = "xlnx,cdma_int";
			interrupt-names = "cdma_introut";
			interrupt-parent = <0x4>;
			interrupts = <0x0 0x5b 0x4>;
			reg = <0x0 0xb0000000 0x0 0x1000>;
			xlnx,addrwidth = <0x28>;

			dma-channel@b0000000 {
				compatible = "xlnx,axi-cdma-channel";
				interrupts = <0x0 0x5b 0x4>;
				xlnx,datawidth = <0x20>;
				xlnx,device-id = <0x0>;
				xlnx,max-burst-len = <0x20>;
			};			
*/

/*
 	dma@b0000000 {
			#dma-cells = <0x1>;
			clock-names = "m_axi_aclk", "s_axi_lite_aclk";
			clocks = <0x3 0x47 0x3 0x47>;
			compatible = "xlnx,axi-cdma-4.1", "xlnx,axi-cdma-1.00.a";
			interrupt-names = "cdma_introut";
			interrupt-parent = <0x4>;
			interrupts = <0x0 0x5b 0x4>;
			reg = <0x0 0xb0000000 0x0 0x1000>;
			xlnx,addrwidth = <0x28>;

			dma-channel@b0000000 {
				compatible = "xlnx,axi-cdma-channel";
				interrupts = <0x0 0x5b 0x4>;
				xlnx,datawidth = <0x400>;
				xlnx,device-id = <0x0>;
				xlnx,max-burst-len = <0x20>;
			};
		};

*/

/* number 48
	Capture_Timer@a0030000 {
			clock-names = "s00_axi_aclk";
			clocks = <0x3 0x47>;
			compatible = "xlnx,Capture-Timer-1.0";
			interrupt-names = "interrupt_out";
			interrupt-parent = <0x4>;
			interrupts = <0x0 0x5c 0x4>;
			reg = <0x0 0xa0030000 0x0 0x10000>;
			xlnx,s00-axi-addr-width = <0x6>;
			xlnx,s00-axi-data-width = <0x20>;
		};
*/

static const struct of_device_id zynq_cdma_of_match[] = {
    { .compatible = "xlnx,cdma_int"},
    { /* end of table */ }
};    
    
MODULE_DEVICE_TABLE(of, zynq_cdma_of_match);


/* ===================================================================
 *
 * zynq_cdma_probe - Initialization method for a zynq_gpio device
 *
 * Return: 0 on success, negative error otherwise.
 */

static int zynq_cdma_probe(struct platform_device *pdev)
{
    struct resource *res;
        
    printk("In probe funtion\n");

    // This code gets the IRQ number by probing the system.

    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
   
    if (!res) {
        printk("No IRQ found\n");
        return 0;
    } 
    
    // Get the IRQ number 
    gic_interrupt  = res->start;

    printk("Probe IRQ # = %lld\n", res->start);

    return 0;

}

/* ======== NOT USED IN THIS CODE ====================================
 *
 * zynq_cdma_remove - Driver removal function
 *
 * Return: 0 always
 */
 
static int zynq_cdma_remove(struct platform_device *pdev)
{
    //struct zynq_gpio *gpio = platform_get_drvdata(pdev)

    return 0;
}


static struct platform_driver zynq_cdma_driver = {
    .driver    = {
        .name = MODULE_NM,
        .of_match_table = zynq_cdma_of_match,
    },
    .probe = zynq_cdma_probe,
    .remove = zynq_cdma_remove,
};


/* ===================================================================
 * function: init_cdma_int
 *
 * This function creates the /proc directory entry cdma_interrupt.
 */
 
static int __init init_cdma_int(void)
{

    int rv = 0;
    int err = 0;
    
    //platform_driver_unregister(&zynq_cdma_driver);
    
   
    printk("Ultra96 Interrupt Module\n");
    printk("Ultra96  Interrupt Driver Loading.\n");
    printk("Using Major Number %d on %s\n", GPIO_MAJOR, MODULE_NM); 

    err = platform_driver_register(&zynq_cdma_driver);
      
    if(err !=0) printk("Driver register error with number %d\n",err);       
    else        printk("Driver registered with no error\n");
    //we do not have minor number while registering the charachter device.
    if (register_chrdev(GPIO_MAJOR, MODULE_NM, &cdma_fops)) {
        printk("cdma_int: unable to get major %d. ABORTING!\n", GPIO_MAJOR);
    goto no_cdma_interrupt;
    }

    proc_cdma_int = proc_create("cdma_interrupt", 0444, NULL, &proc_fops );
    msg=kmalloc(GFP_KERNEL,10*sizeof(char)); //memory allocation for message printing    
    if(proc_cdma_int == NULL) {
          printk("cdma_int: create /proc entry returned NULL. ABORTING!\n");
    goto no_cdma_interrupt;
    }

    // Request interrupt
    
    // rv = request_irq(gic_interrupt, cdma_int_handler, IRQF_TRIGGER_RISING,
    

    rv = request_irq(gic_interrupt, 
                    (irq_handler_t) cdma_int_handler, 
                     IRQF_TRIGGER_RISING,
                    "cdma_interrupt", NULL);
  
    if (rv) {
       // printk("Can't get interrupt %d\n", INTERRUPT);
        printk("Can't get interrupt %d\n", gic_interrupt);
    goto no_cdma_interrupt;
    }

    printk(KERN_INFO "%s %s Initialized\n",MODULE_NM, MODULE_VER);
    
    return 0;

    // remove the proc entry on error
    
no_cdma_interrupt:
    kfree(msg);
    printk(KERN_INFO "%s %s removed\n", MODULE_NM, MODULE_VER);
    free_irq(gic_interrupt,NULL);                   // Release IRQ    
    unregister_chrdev(GPIO_MAJOR, MODULE_NM);       // Release character device
    unregister_chrdev(GPIO_MAJOR, MODULE_NM);
    platform_driver_unregister(&zynq_cdma_driver);
    remove_proc_entry("cdma_interrupt", NULL);
    return -EBUSY;
};

/* ===================================================================
 * function: cleanup_cdma_interrupt
 *
 * This function frees interrupt then removes the /proc directory entry 
 * cdma_interrupt. 
 */
 
static void __exit cleanup_cdma_interrupt(void)
{

    free_irq(gic_interrupt,NULL);                   // Release IRQ    
    unregister_chrdev(GPIO_MAJOR, MODULE_NM);       // Release character device
    platform_driver_unregister(&zynq_cdma_driver);  // Unregister the driver
    remove_proc_entry("cdma_interrupt", NULL);      // Remove process entry
    kfree(msg);
    printk(KERN_INFO "%s %s removed\n", MODULE_NM, MODULE_VER);
     
}


/* ===================================================================
 *
 *
 *
 */


module_init(init_cdma_int);
module_exit(cleanup_cdma_interrupt);

MODULE_AUTHOR("Mark McDermott");
MODULE_DESCRIPTION("cdma_interrupt proc module");
MODULE_LICENSE("GPL");

