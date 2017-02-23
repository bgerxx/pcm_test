#include <asm/processor.h>
#define RDRAND_LONG    ".byte 0x48,0x0f,0xc7,0xf0"
#define RDRAND_RETRY_LOOPS      10

bool rdrand_long(unsigned long *v)
{
        bool ok;
        unsigned int retry = RDRAND_RETRY_LOOPS;
        do {
                asm volatile(RDRAND_LONG "\n\t"
                             "setc %0"
                             : "=qm" (ok), "=a" (*v));
                if (ok)
                        return true;
        } while (--retry);
        return false;
}


void rdrand_test(void)
{
	unsigned long v;
	rdrand_long(&v);
	printk("get_rdrand=%lx\n",v);
}
