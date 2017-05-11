#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "pcm_test_ioctl.h"

#define CMD_LINE_ARGS "ht:"
#define BUFFSIZE 10

void print_help(void)
{
}

int main(int argc, char *argv[])
{
	int fd, err, ch;
	unsigned long ins_ioc = CPUID;
        char type[BUFFSIZE] = "cpuid";
	
        while ((ch= getopt(argc, argv, CMD_LINE_ARGS)) != -1) {
                switch (ch) {
                case 't':
		        fd = open("/dev/pcm_test",O_RDWR);
        		if (fd < 0){
                		printf("%s\n", strerror(errno));
                		return 1;
        		}
                        strncpy(type,optarg,sizeof(type));
                        break;
                case 'h':
                        print_help();
                        return 1;
                }
        }
        if (strcmp(type,"cpuid") == 0) {
                ins_ioc = CPUID;
        }
	else if (strcmp(type,"rdpmc") == 0) {
                ins_ioc = RDPMC;
        }
        else if (strcmp(type,"rdrand") == 0) {
                ins_ioc = RDRAND;
        }
        else if (strcmp(type,"rdseed") == 0) {
                ins_ioc = RDSEED;
        }
        else if (strcmp(type,"rdtsc") == 0) {
                ins_ioc = RDTSC;
        }
        else if (strcmp(type,"rdtscp") == 0) {
                ins_ioc = RDTSCP;
        }
        else if (strcmp(type,"pio") == 0) {
                ins_ioc = PIO;
        }
        else if (strcmp(type,"mmio") == 0) {
                ins_ioc = MMIO;
        }
        else if (strcmp(type,"icmp") == 0) {
                ins_ioc = ICMP;
        }
	
	err = ioctl(fd, ins_ioc);
	if (err != 0){
		printf("%s\n", strerror(errno));
		return 1;
	}
	close(fd);
	return 0;
}
