#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "pcm_test_ioctl.h"

int main(int argc, char *argv[])
{
	int fd, err;
	fd = open("/dev/pcm_test",O_RDWR);
	if (fd < 0){
		printf("%s\n", strerror(errno));
		return 1;
	}
	err = ioctl(fd, TSC_RW);
	if (err != 0){
		printf("%s\n", strerror(errno));
		return 1;
	}
	close(fd);
	return 0;
}
