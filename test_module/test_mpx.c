#ifndef _TEST_PT_H_
#define _TEST_PT_H_
#include "test_mpx.h"
#include <asm/msr.h>
#include <linux/types.h>

//int test_mpx_rdmsr(void);
//int test_mpx_wrmsr(void);


void test_mpx_bndmk(void) {
    // clear_bnd0
    long size = 0;
    void *ptr = NULL;
    printk("To run bndmk.\n");
    /* F3 0F 1B /r BNDMK bnd, m64           */
    /* f3 0f 1b 04 11    bndmk  (%rcx,%rdx,1),%bnd0 */
    asm volatile(".byte 0xf3,0x0f,0x1b,0x04,0x11\n\t"
                 : : "c" (ptr), "d" (size-1)
                 :   "memory");
	printk("bndmk executed.\n");
}

void test_mpx_bndcl(void) {
    void *ptr = NULL;
    /* F3 0F 1A /r  NDCL bnd, r/m64         */
    /* f3 0f 1a 01  bndcl  (%rcx),%bnd0     */
    asm volatile(".byte 0xf3,0x0f,0x1a,0x01\n\t"
                : : "c" (ptr)
                :   "memory");

    printk("bndcl executed.\n");
}



void test_mpx_bndcu(void) {
    void *ptr = NULL;
    /* F2 0F 1A /r  BNDCU bnd, r/m64    */
    /* f2 0f 1a 01  bndcu  (%rcx),%bnd0 */
    asm volatile(".byte 0xf2,0x0f,0x1a,0x01\n\t"
                : : "c" (ptr)
                :   "memory");

    printk("bndcu executed.\n");
}

void test_mpx_bndcn(void) {
    void *ptr = NULL;
    /* F2 0F 1B /r BNDCN bnd, r/m64*/
    asm volatile(".byte 0xf2,0x0f,0x1b,0x01\n\t"
                : : "c" (ptr)
                :   "memory");
    printk("bndcn executed.\n");
}

void test_mpx_bndmov(void) {
    /* 66 0F 1B /r  BNDMOV bnd1/m128, bnd2  */
    /* 66 0f 1b c2  bndmov %bnd0,%bnd2  */

    asm volatile(".byte 0x66,0x0f,0x1b,0xc2\n\t");

}


int test_mpx_rdmsr(void) {
	u32 data_low,data_high;
	int err;
	err = rdmsr_safe(MSR_IA32_BNDCFGS, &data_low, &data_high);
	printk("return value of rdmsr_safe is %d\n", err);
	if (err) {
		printk(KERN_ERR "%s return err %d\n", __func__, err);
	}
	return err;

}
int test_mpx_wrmsr(void) {
	u32 data_low = 0, data_high = 0;
	int err;
	err = wrmsr_safe(MSR_IA32_BNDCFGS, data_low, data_high);
	if (err) {
		printk(KERN_ERR "%s return err %d\n", __func__, err);
	}
	return err;
}

#endif
