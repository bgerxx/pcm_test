#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <asm/processor.h>
#include <asm/msr.h>
#include "pcm_tc.h"
#include "pcm_test_ioctl.h"

#define KVM_HYPERCALL ".byte 0x0f,0x01,0xc1"

static long hypercall(unsigned int nr)
{
        long ret;
        asm volatile(KVM_HYPERCALL
             : "=a"(ret)
             : "a"(nr)
             : "memory");
        return ret;
}

#define PCM_CDEV_NAME "pcm_test"
static int pcm_cdev_major;
static struct class *pcm_class;

struct pcm_cdev {
	struct cdev cdev;
};

struct pcm_cdev * pcm_cdev_t;

static int test_tsc(void)
{
	u64 t1, t2;
        rdtscll(t1);
	rdtscll(t2);
	printk("rdtsc latency %u\n", (unsigned)(t2 - t1));

	test_wrtsc(0);
	test_wrtsc(100000000000ull);
/*
	if (check_cpuid_80000001_edx(CPUID_80000001_EDX_RDTSCP)) {
        	test_rdtscp(0);
        	test_rdtscp(10);
        	test_rdtscp(0x100);
	} else
        	printk("rdtscp not supported\n");
		return 1;
*/
	return 0;
}

static int pcm_cdev_open(struct inode *inode, struct file *filp)
{
	struct pcm_cdev *dev; 
        printk("%s()\n", __func__);

        dev = container_of(inode->i_cdev, struct pcm_cdev, cdev);
        filp->private_data = dev; 
	return 0;
}

static int pcm_cdev_release(struct inode *inode, struct file *filp)
{
        printk("%s()\n", __func__);
        return 0;
}

static long pcm_cdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	//struct pcm_cdev *dev = filp->private_data;
	int err = 0;
	printk("%s()\n", __func__);
	//if(copy_from_user(op, (void *)arg, sizeof(struct ptl_ep_cdev_op)))
	//	return -EFAULT;
	if (_IOC_TYPE(cmd) != PCM_TEST_IOC_MAGIC) 
        	return -EINVAL;
        if (_IOC_NR(cmd) > PCM_TEST_IOC_MAXNR) 
        	return -EINVAL;

    	if (_IOC_DIR(cmd) & _IOC_READ)
        	err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    	else if (_IOC_DIR(cmd) & _IOC_WRITE)
        	err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
    	if (err) 
        	return -EFAULT;
	local_irq_disable();
        hypercall(102);
	switch(cmd) {
		case TSC_RW:
			test_tsc();
			break;
		default:
			break;
	}
	hypercall(103);
        local_irq_enable();
	return err;
}

static const struct file_operations pcm_cdev_file_ops = {
        .owner = THIS_MODULE,
        .open = pcm_cdev_open,
        .unlocked_ioctl = pcm_cdev_ioctl,
        .release = pcm_cdev_release,
};

static int pcm_cdev_init(void)
{
        int ret;
        dev_t pcm_cdev_no =  MKDEV(pcm_cdev_major, 0);
        if ((ret = alloc_chrdev_region(&pcm_cdev_no, 0, 1, PCM_CDEV_NAME)) < 0) {
                printk("alloc_chrdev_region error\n");
                return ret;
        }
        pcm_cdev_major = MAJOR(pcm_cdev_no);
        return ret;
}

static void pcm_cdev_exit(void)
{
	device_destroy(pcm_class,MKDEV(pcm_cdev_major, 0));
	class_destroy(pcm_class);
	cdev_del(&pcm_cdev_t->cdev);
	kfree(pcm_cdev_t);
        unregister_chrdev_region(MKDEV(pcm_cdev_major, 0), 1);
}

static int pcm_cdev_add(void)
{
	int ret;
        dev_t dev_no = MKDEV(pcm_cdev_major, 0);
	pcm_class = class_create(THIS_MODULE, PCM_CDEV_NAME);
        pcm_cdev_t = (struct pcm_cdev*)kmalloc(sizeof(struct pcm_cdev), GFP_KERNEL);

        cdev_init(&pcm_cdev_t->cdev, &pcm_cdev_file_ops);
	pcm_cdev_t->cdev.owner = THIS_MODULE;
	pcm_cdev_t->cdev.ops = &pcm_cdev_file_ops;

        ret = cdev_add(&pcm_cdev_t->cdev, dev_no, 1);
        if(ret < 0) {
                printk("Fail to cdev_add\n");
                return ret;
        }
	
	device_create(pcm_class, NULL, MKDEV(pcm_cdev_major, 0), NULL, PCM_CDEV_NAME);

        return ret;
}

static int __init pcm_test_init(void)
{
	int ret;
	pcm_cdev_init();
	ret = pcm_cdev_add();
	return ret;
}

static void __exit pcm_test_exit(void)
{
	pcm_cdev_exit();
}

MODULE_LICENSE("GPL");
module_init(pcm_test_init);
module_exit(pcm_test_exit);

