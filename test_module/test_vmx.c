#include "test_vmx.h"

void test_vmxoff(void)
{
	asm volatile ("vmxoff\n" ::: "cc");
	//__write_cr4(__read_cr4() & ~X86_CR4_VMXE);
	//write_cr0(read_cr0() | X86_CR0_CD);
}

void test_vmxon(u64 addr)
{
	u64 old, test_bits;

	rdmsrl(MSR_IA32_FEATURE_CONTROL, old);
	test_bits = FEATURE_CONTROL_LOCKED;
	test_bits |= FEATURE_CONTROL_VMXON_ENABLED_OUTSIDE_SMX;
	if ((old & test_bits) != test_bits) {
		wrmsrl(MSR_IA32_FEATURE_CONTROL, old | test_bits);
	}

        /* Mask this out, since this would inject#GP to guest in non-nested mode */
	__write_cr4(__read_cr4() | X86_CR4_VMXE);

	asm volatile (ASM_VMX_VMXON_RAX
			: : "a"(&addr), "m"(addr)
			: "memory", "cc");
	printk("vmxon succeed\n");
	return;
}
