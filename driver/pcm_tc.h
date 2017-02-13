#include <linux/types.h>

#ifndef __PCM_TEST_MOD_H_
#define __PCM_TEST_MOD_H_


/*TEST_TSC*/
#define CPUID_80000001_EDX_RDTSCP	    (1 << 27)
int check_cpuid_80000001_edx(unsigned int bit);
void test_wrtsc(u64 t1);
//void test_rdtscp(unsigned int aux);



#endif
