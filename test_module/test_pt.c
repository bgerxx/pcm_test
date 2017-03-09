#include <asm/msr.h>
#include <linux/types.h>
#include "test_pt.h"

int test_pt_rdmsr(void)
{

	u32 msr_val, data_low, data_high;
	int err;
	printk("test rdmsr 0x570\n");
	msr_val = IA32_RTIT_CTL_MSR;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x571\n");
	msr_val = IA32_RTIT_STATUS_MSR;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x572\n");
	msr_val = IA32_RTIT_CR3_MATCH;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x560\n");
	msr_val = IA32_RTIT_OUTPUT_BASE;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x561\n");
	msr_val = IA32_RTIT_OUTPUT_MASK;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x580\n");
	msr_val = IA32_RTIT_ADDR0_A;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x581\n");
	msr_val = IA32_RTIT_ADDR0_B;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x582\n");
	msr_val = IA32_RTIT_ADDR1_A;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x583\n");
	msr_val = IA32_RTIT_ADDR1_B;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x584\n");
	msr_val = IA32_RTIT_ADDR2_A;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x585\n");
	msr_val = IA32_RTIT_ADDR2_B;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x586\n");
	msr_val = IA32_RTIT_ADDR3_A;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("test rdmsr 0x587\n");
	msr_val = IA32_RTIT_ADDR3_B;
	err = rdmsr_safe(msr_val, &data_low, &data_high);
	if (err)
		goto fail;	

	printk("rdmsr for PT msrs success\n");
	return 0;
fail:
	printk("rdmsr 0x%x err %d\n", msr_val, err);
	return 1;
}

int test_pt_wrmsr(void)
{
	u32 msr_val;
	u32 data_low = 0x0;
	u32 data_high = 0x0;
	int err;
	printk("test wrmsr 0x570\n");
	msr_val = IA32_RTIT_CTL_MSR;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x571\n");
	msr_val = IA32_RTIT_STATUS_MSR;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x572\n");
	msr_val = IA32_RTIT_CR3_MATCH;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x560\n");
	msr_val = IA32_RTIT_OUTPUT_BASE;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x561\n");
	msr_val = IA32_RTIT_OUTPUT_MASK;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x580\n");
	msr_val = IA32_RTIT_ADDR0_A;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x581\n");
	msr_val = IA32_RTIT_ADDR0_B;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x582\n");
	msr_val = IA32_RTIT_ADDR1_A;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x583\n");
	msr_val = IA32_RTIT_ADDR1_B;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x584\n");
	msr_val = IA32_RTIT_ADDR2_A;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x585\n");
	msr_val = IA32_RTIT_ADDR2_B;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x586\n");
	msr_val = IA32_RTIT_ADDR3_A;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("test wrmsr 0x587\n");
	msr_val = IA32_RTIT_ADDR3_B;
	err = wrmsr_safe(msr_val, data_low, data_high);
	if (err)
		goto fail;	

	printk("wrmsr success for PT MSRs\n");

	return 0;
fail:
	printk("wrmsr 0x%x err %d\n", msr_val, err);
	return 1;
}
