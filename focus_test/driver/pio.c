#include <asm/pci-direct.h>

void pio_test(void)
{
	u8 bus = 0;
	u8 slot = 2;
	u8 func = 0;
	u8 offset =0;
	read_pci_config(bus, slot, func, offset);
}
