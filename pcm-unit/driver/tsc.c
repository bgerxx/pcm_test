#include <linux/types.h>
#include <asm/msr.h>
#include <asm/processor.h>

int check_cpuid_80000001_edx(unsigned int bit)
{
	u32 eax, ebx, ecx, edx;
	cpuid(0x80000001, &eax, &ebx, &ecx, &edx);
	return (edx & bit) != 0;
}


void test_wrtsc(u64 t1)
{
	u64 t2;

/*	write_tsc(t1, t1>>32);
 */
	native_write_msr_safe(MSR_IA32_TSC, t1, t1>>32);
	rdtscll(t2);
	printk("rdtsc after wrtsc%lld : %lld \n", t1, t2);
}

/*
void test_rdtscp(unsigned int aux)
{

       unsigned int ecx;

       write_rdtscp_aux(aux);
*/
void test_rdtscp(u64 aux)
{
	u32 ecx;

	native_write_msr_safe( MSR_TSC_AUX, aux, aux>>32);
	rdtscpll(ecx, aux);
	printk("Test RDTSCP eax=%d, aux=%lld", ecx, aux);
}

