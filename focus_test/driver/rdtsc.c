#include <asm/msr.h>

void rdtsc_test(void)
{
	unsigned long long val;
	val = rdtsc();
	printk("rdtsc=%llx\n",val);
}
