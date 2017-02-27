#include <asm/msr.h>

/*unsigned long long native_read_tscp(unsigned int *aux)
{
	unsigned long low, high;
	asm volatile(".byte 0x0f,0x01,0xf9"
		: "=a" (low), "=d" (high), "=c" (*aux));
	return low | ((u64)high << 32);
}*/


#define rdtscpll(val, aux) (val) = native_read_tscp(&(aux))

void rdtscp_test(void)
{
	unsigned long long val;
	unsigned int aux;
	rdtscpll(val, aux);
	printk("rdtscp=%llx\n",val);
}
