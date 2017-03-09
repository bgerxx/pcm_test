#include <linux/ioctl.h>

#ifndef __PCM_TEST_IOCTL_H__
#define __PCM_TEST_IOCTL_H__


#define PCM_TEST_IOC_MAGIC  'k'

#define TSC_RW _IO(PCM_TEST_IOC_MAGIC, 1)
#define SETJMP _IO(PCM_TEST_IOC_MAGIC, 2)
#define MSR_RW _IO(PCM_TEST_IOC_MAGIC, 3)

#define PCM_TEST_IOC_MAXNR 2


#endif
