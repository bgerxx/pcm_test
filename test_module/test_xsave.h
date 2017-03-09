#ifndef _TEST_XSAVE_H_
#define _TEST_XSAVE_H_
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/msr.h>
#include <asm/msr-index.h>
#include <asm/special_insns.h>
#include <asm/processor-flags.h>

#define XSTATE_MASK      0x1


void test_xsaves(void);
void test_xsetbv(void);

#endif


