#include <linux/ioctl.h>

#ifndef __PCM_TEST_IOCTL_H__
#define __PCM_TEST_IOCTL_H__


#define PCM_TEST_IOC_MAGIC  'k'
#define CPUID _IO(PCM_TEST_IOC_MAGIC, 1)
#define INVD _IO(PCM_TEST_IOC_MAGIC, 2)
#define RDSEED _IO(PCM_TEST_IOC_MAGIC, 3)
#define RDRAND _IO(PCM_TEST_IOC_MAGIC, 4)
#define RDPMC _IO(PCM_TEST_IOC_MAGIC, 5)
#define RDTSC _IO(PCM_TEST_IOC_MAGIC, 6)
#define RDTSCP _IO(PCM_TEST_IOC_MAGIC, 7)
#define PCM_TEST_IOC_MAXNR 7


#endif
