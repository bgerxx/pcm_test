#include <asm/msr.h>

void rdpmc_test(void)
{
	unsigned long long val;
	rdpmcl(0, val);
} 
