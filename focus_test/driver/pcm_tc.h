#include <linux/types.h>

#ifndef __PCM_TEST_MOD_H_
#define __PCM_TEST_MOD_H_


/*TEST CPUID*/
void cpuid_test(void);

/*TEST RDPMC*/
void rdpmc_test(void);

/*TEST INVD*/
void invd_test(void);

/*TEST RDRAND*/
void rdrand_test(void);
void rdseed_test(void);

/*TEST RDTSC*/
void rdtsc_test(void);

/*TEST RDTSCP*/
void rdtscp_test(void);

/*TEST PIO*/
void pio_test(void);

/*TEST MMIO*/
void mmio_test(void);

/*TEST ICMP*/
void icmp_test(void);

#endif
