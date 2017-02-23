#include <asm/processor.h>
#define RDSEED_LONG    ".byte 0x48,0x0f,0xc7,0xf8"

bool rdseed_long(unsigned long *v)
{
        bool ok;
        asm volatile(RDSEED_LONG "\n\t"
                     "setc %0"
                     : "=qm" (ok), "=a" (*v));
        return ok;
}

void rdseed_test(void)
{
	unsigned long v;
	rdseed_long(&v);
	printk("get_rdseed=%lx\n",v);
}
