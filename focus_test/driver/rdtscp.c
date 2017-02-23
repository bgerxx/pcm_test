#include <asm/msr.h>

void rdtscp_test(void)
{
	unsigned long long val;
	unsigned int *aux;
	rdtscpll(val, aux);
	printk("rdtscp=%llx\n",val);
}
