#ifndef _TEST_MPX_H_
#define _TEST_MPX_H_

#define MSR_IA32_BNDCFGS 0x00000d90
int test_mpx_rdmsr(void);
int test_mpx_wrmsr(void);
void test_mpx_bndmk(void);
void test_mpx_bndcl(void);
void test_mpx_bndcu(void);
void test_mpx_bndcn(void);
void test_mpx_bndmov(void);
void test_mpx_bndldx(void);
void test_mpx_bndstx(void);
#endif
