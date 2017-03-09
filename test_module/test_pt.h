#ifndef _TEST_PT_H_
#define _TEST_PT_H_

#define IA32_RTIT_CTL_MSR 0x570
#define IA32_RTIT_STATUS_MSR 0x571
#define IA32_RTIT_CR3_MATCH 0x572
#define IA32_RTIT_OUTPUT_BASE 0x560
#define IA32_RTIT_OUTPUT_MASK 0x561
#define IA32_RTIT_ADDR0_A  0x580
#define IA32_RTIT_ADDR0_B  0x581
#define IA32_RTIT_ADDR1_A  0x582
#define IA32_RTIT_ADDR1_B  0x583
#define IA32_RTIT_ADDR2_A  0x584
#define IA32_RTIT_ADDR2_B  0x585
#define IA32_RTIT_ADDR3_A  0x586
#define IA32_RTIT_ADDR3_B  0x587
int test_pt_rdmsr(void);
int test_pt_wrmsr(void);

#endif
