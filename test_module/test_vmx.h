#ifndef _TEST_VMX_H_
#define _TEST_VMX_H_
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/msr.h>
#include <asm/msr-index.h>
#include <asm/special_insns.h>
#include <asm/processor-flags.h>

#define ASM_VMX_VMXON_RAX ".byte 0xf3, 0x0f, 0xc7, 0x30"

void test_vmxon(u64 addr);
void test_vmxoff(void);

#endif /* _TEST_VMX_H_ */
