#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <asm/mwait.h>
#include <asm/thread_info.h>
#include "test_vmx.h"
#include "test_pt.h"
#include "test_rdpmc.h"
#include "test_mpx.h"
#include "test_xsave.h"

#define DRIVER_AUTHOR "Lizhen You"
#define DRIVER_DESC "test driver for cpu instructions"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

static int tc_no = 0; 
char *vmxon_region;
u64 vmxon_phy_region;

module_param(tc_no, int, 0);
MODULE_PARM_DESC(tc_no, "testcase no.");

/*
 * tc_no testcase number 
 * tc_no 1 : test vmxon
 *   inject #UD to guest
 * tc_no 2 : test vmxoff
 *   inject #UD to guest
 * tc_no 3: rsmsr/wrmsr of PT msrs
 *   inject #GP to guest
 * tc_no 4: test rdpmc
 *   checking the "dmesg" output
 * tc_no 5: test invd
 *   checking the "dmesg" for rip addr
 * tc_no 6: test getsec
 *   return #UD to guest
 * tc_no 7: test monitor
 *   inject #UD to guest
 * tc_no 8: test mwait
 *   inject #UD to guest
 * tc_no 9: remove mpx
 *   inject #GP to access of MSR_IA32_BNDCFGS
 * tc_no 10: test xsaves
 *   inject #UD to guest
 * tc_no 11: test mpx bndmk  
 *   NOP
*/


static void test_invd(void)
{
    asm volatile("invd");
    printk("invd executed\n");
}


static void test_getsec(void)
{
	//__write_cr4(__read_cr4() | X86_CR4_SMXE);
	asm volatile("getsec":: "a"(0));
	printk("getsec executed\n");
}
static void test_mwait(void)
{
	unsigned long eax;
	eax = 0xf;
	__mwait(eax, 0);
	printk("mwait executed\n");
}

static void test_monitor(void)
{
	__monitor((void *)&current_thread_info()->flags ,0 ,0);
	printk("monitor executed\n");
}

static int __init init_test_mode(void)
{
	printk("tc_no is %d\n", tc_no);

	switch (tc_no){
	    case 1:
		vmxon_region = (char *)kmalloc(0x1000, GFP_KERNEL);
		vmxon_phy_region = __pa(vmxon_region);
		test_vmxon(vmxon_phy_region);
		break;
	     case 2: 
		test_vmxoff();
	     case 3: /* PT */
		test_pt_rdmsr();
		test_pt_wrmsr();
		break;
	     case 4: /* rdpmc */
		test_rdpmc();
		break;
	     case 5: /* test invd */
		test_invd();
		break;
	     case 6:
		test_getsec();
		break;
	     case 7:
		test_monitor();
	     case 8:
		test_mwait();
		break;
	     case 9:
		printk(KERN_ERR "test_mpx\n");
		test_mpx_rdmsr();
		test_mpx_wrmsr();
		break;
             case 10:
                test_xsaves();
                break;         
             case 11:
                test_mpx_bndmk();
                break;
            case 12:
                test_mpx_bndcl();
                break;
            case 13:
                test_mpx_bndcu();
                break;    
            case 14:
                test_mpx_bndcn();
                break;
            case 15:
                test_mpx_bndmov();
                break;
            case 18:
                test_xsetbv();
                break;
	     default:
		printk("Invalid tc_no %d\n", tc_no);
		break;

	}
	return 0;
}

static void __exit exit_test_mode(void)
{
	printk("test_module cleanup\n");
	if (vmxon_region)
		kfree(vmxon_region);
}
module_init(init_test_mode);
module_exit(exit_test_mode);
