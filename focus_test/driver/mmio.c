#include <asm/io.h>

void mmio_test(void)
{
	void *addr;
	addr = ioremap(0xfebc0000,4);
	printk("addr:%x, readl:%x\n", addr, readl(addr));
}
