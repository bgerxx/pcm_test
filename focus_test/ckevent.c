#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#pragma pack(push)
#pragma pack(1)
struct exec_pos {
        uint32_t num_branches;
        uint64_t eip;
        uint64_t ecx;
};


typedef struct _event_log_ {
        uint64_t        next_addr;      // physical address of next event
        uint32_t        log_type;
        uint32_t        etype;
        struct          exec_pos pos;
        uint32_t        log_data[0];    // log data, format depending on
} event_log_t;

struct event_log_cpuid_read {
        uint32_t in_eax;
        uint32_t in_ecx;
        uint32_t out_eax;
        uint32_t out_ebx;
        uint32_t out_ecx;
        uint32_t out_edx;
};
#pragma pack(pop)


int main(int argc, char *argv[])
{
        event_log_t *event_t;
	struct event_log_cpuid_read *event_cpuid;
	event_t = (event_log_t *)malloc(sizeof(*event_t)+sizeof(*event_cpuid));
	memset(event_t, 0, sizeof(*event_t)+sizeof(*event_cpuid));
	
	FILE *fd = fopen("/tmp/event.bin", "r");
	fread(event_t,sizeof(*event_t)+sizeof(*event_cpuid),1,fd);

	event_cpuid = (struct event_log_cpuid_read *)(event_t->log_data);
	
	printf("log_type= %llx\n",event_cpuid->out_ebx);
        fclose(fd);
	free(event_t);
        return 0;
}

