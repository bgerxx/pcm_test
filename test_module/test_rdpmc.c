#include "test_rdpmc.h"
static uint64_t asm_rdpmc(int counter)
{
	uint32_t a, d;
	asm volatile ("rdpmc" : "=a" (a), "=d" (d) : "c" (counter));
	return (uint64_t)d << 32 | a;
}

void test_rdpmc(void)
{
	int i = 0;
	uint64_t ret;

	/* for skylake, the valid pmc index is 0 ~ 3 */
	for (i = 0; i < 4; i++) {
		ret = asm_rdpmc(i);
		if (ret) {
			printk("rdpmc fail with value %llu", ret);
		}	
	}	
	printk("rdpmc test PASS!\n");
}
