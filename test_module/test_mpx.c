#ifndef _TEST_PT_H_
#define _TEST_PT_H_
#include "test_mpx.h"
#include <asm/msr.h>
#include <linux/types.h>

//int test_mpx_rdmsr(void);
//int test_mpx_wrmsr(void);



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
