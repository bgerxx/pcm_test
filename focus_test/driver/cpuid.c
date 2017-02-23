#include <asm/processor.h>

void cpuid_test(void)
{
	cpuid_eax(0);
}
