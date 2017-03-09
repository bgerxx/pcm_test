#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/bootmem.h>
#include <asm/processor.h>
//#include <asm/xcr.h>
#include "test_xsave.h"
#define REX_PREFIX      "0x48, "
#define XSAVES          ".byte " REX_PREFIX "0x0f,0xc7,0x2f"
#define XRSTORS         ".byte " REX_PREFIX "0x0f,0xc7,0x1f"
#define XCR_XFEATURE_ENABLED_MASK       0x00000000
#define XSTATE_CPUID            0x0000000d
void test_xsaves(void)
{
	u64 mask = XSTATE_MASK;
	u32 lmask = mask;
	u32 hmask = mask >> 32;
        char * fx;
	//struct xsave_struct *fx;
 	//fx = alloc_bootmem_align(0x000003c0,
        //                   __alignof__(struct xsave_struct));
        //
	fx = (char *)kmalloc(0x00000440, GFP_KERNEL);
	asm volatile("1:"XSAVES"\n\t"
              "2:\n\t"
              : : "D" (fx), "m" (*fx), "a" (lmask), "d" (hmask)
              :   "memory");
	printk("xsaves is executed.\n");
	/*asm volatile("1:"XRSTORS"\n\t"
              "2:\n\t"
              : : "D" (fx), "m" (*fx), "a" (lmask), "d" (hmask)
              :   "memory");*/

}

void test_xsetbv(void)
{
    u32 eax, ebx, ecx, edx;
    cpuid_count(XSTATE_CPUID, 0, &eax, &ebx, &ecx, &edx);
    printk("Begin to test legal xsetbv:\n");
    printk("eax %d", eax);
    printk("edx %d", edx);
    asm volatile(".byte 0x0f,0x01,0xd1" /* xsetbv */
                : : "a" (eax), "d" (edx), "c" (XCR_XFEATURE_ENABLED_MASK));
    printk("Legal xsetbv test is executed.\n");

    printk("Begin to test illegal xsetbv:\n");
    edx += 1;
    printk("eax %d", eax);
    printk("edx %d", edx);

    asm volatile(".byte 0x0f,0x01,0xd1" /* xsetbv */
                        : : "a" (eax), "d" (edx), "c" (XCR_XFEATURE_ENABLED_MASK));
    printk("Illegal xsetbv test is executed.\n");
}

   
