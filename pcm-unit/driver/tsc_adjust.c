#include <linux/types.h>
#include <asm/msr.h>
#include <asm/processor.h>

#include "test_func.h"

#define IA32_TSC_ADJUST 0x3b

void test_tsc_adjust()
{
	u64 t1, t2, t3, t4, t5, p;
	u64 est_delta_time;

    u32 eax, ebx, ecx, edx;
    cpuid(7, &eax, &ebx, &ecx, &edx);

    if (ebx & (1 << 1)) {
//	if (cpuid(7).b & (1 << 1)) { // IA32_TSC_ADJUST Feature is enabled?

        rdmsrl_safe(IA32_TSC_ADJUST, &p);
		printk("IA32_TSC_ADJUST msr initialization,  %lld\n", p);//,
//				rdmsr(IA32_TSC_ADJUST) == 0x0);
		t3 = 100000000000ull;
//		t1 = rdtsc();
//		wrmsr(IA32_TSC_ADJUST, t3);
//		t2 = rdtsc();
        rdtscll(t1);		
        wrmsrl_safe(IA32_TSC_ADJUST, t3);
        rdtscll(t2);		
        p = 0x0;
        rdmsrl_safe(IA32_TSC_ADJUST, &p);
        t3 = p;
        printk("IA32_TSC_ADJUST msr read / write, %lld\n", t3);
//		report("IA32_TSC_ADJUST msr read / write",
//				rdmsr(IA32_TSC_ADJUST) == t3);
        printk("TSC adjustment for IA32_TSC_ADJUST value, t2-t1=%lld\n", (t2 - t1));				
//		report("TSC adjustment for IA32_TSC_ADJUST value",
//				(t2 - t1) >= t3);
		t3 = 0x0;
//		wrmsr(IA32_TSC_ADJUST, t3);
//		report("IA32_TSC_ADJUST msr read / write",
//				rdmsr(IA32_TSC_ADJUST) == t3);
        wrmsrl_safe(IA32_TSC_ADJUST, t3);
        rdmsrl_safe(IA32_TSC_ADJUST, &t3);
        printk("IA32_TSC_ADJUST msr read / write, %lld\n", t3);
        t4 = 100000000000ull;
//		t1 = rdtsc();
//		wrtsc(t4);
//		t2 = rdtsc();
//		t5 = rdmsr(IA32_TSC_ADJUST);
        rdtscll(t1);
        wrmsrl_safe(MSR_IA32_TSC, t4);
        rdtscll(t2);
        rdmsrl_safe(IA32_TSC_ADJUST, &t5);
        // est of time between reading tsc and writing tsc,
		// (based on IA32_TSC_ADJUST msr value) should be small
		est_delta_time = t4 - t5 - t1;
		// arbitray 2x latency (wrtsc->rdtsc) threshold
        printk("IA32_TSC_ADJUST msr adjustment on tsc write, est_delta_time=%lld, stdvalue=%lld\n", est_delta_time, 2 * (t2 - t4));
//		report("IA32_TSC_ADJUST msr adjustment on tsc write",
//				est_delta_time <= (2 * (t2 - t4)));
	}
	else {
//		report("IA32_TSC_ADJUST feature not enabled", true);
        printk("IA32_TSC_ADJUST feature not enabled\n");		
	}
//	return report_summary();
}
