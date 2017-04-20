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

struct event_log_rdtsc {
        uint64_t tsc;
};

struct event_log_rdtscp {
        uint64_t tsc;
        uint64_t tsc_aux;
};

struct event_log_rdpmc {
        uint64_t pmc;
};

struct event_log_rdseed {
        uint64_t value;
};

struct event_log_msr_read {
        uint32_t in_ecx;
        uint32_t out_eax;
        uint32_t out_edx;
};

struct event_log_pio_ins {
        uint32_t rep_num;
	uint32_t in_data[0];
};

struct event_log_mmio_ins {
        uint32_t rep_num;
        uint64_t in_data[0];
};

struct event_log_mmio_outs {
        uint32_t rep_num;
        uint64_t out_data[0];
};

#pragma pack(pop)

#define CMD_LINE_ARGS "ht:"
#define BUFFSIZE 10

void print_help()
{
}

void ck_cpuid(void)
{
        event_log_t *event_t;
        struct event_log_cpuid_read *event_cpuid;
        event_t = (event_log_t *)malloc(sizeof(*event_t)+sizeof(*event_cpuid));
        memset(event_t, 0, sizeof(*event_t)+sizeof(*event_cpuid));
        FILE *fd = fopen("/tmp/event.bin", "r");
        fread(event_t,sizeof(*event_t)+sizeof(*event_cpuid),1,fd);
        event_cpuid = (struct event_log_cpuid_read *)(event_t->log_data);

        printf("in_eax=%llx\n",event_cpuid->in_eax);
        printf("in_ecx=%llx\n",event_cpuid->in_ecx);
        printf("out_eax=%llx\n",event_cpuid->out_eax);
        printf("out_ebx=%llx\n",event_cpuid->out_ebx);
        printf("out_ecx=%llx\n",event_cpuid->out_ecx);
        printf("out_edx=%llx\n",event_cpuid->out_edx);

        fclose(fd);
        free(event_t);
}

void ck_rdpmc(void)
{
	event_log_t *event_t;
        struct event_log_rdpmc *event_rdpmc;
        event_t = (event_log_t *)malloc(sizeof(*event_t)+sizeof(*event_rdpmc));
        memset(event_t, 0, sizeof(*event_t)+sizeof(*event_rdpmc));
        FILE *fd = fopen("/tmp/event.bin", "r");
        fread(event_t,sizeof(*event_t)+sizeof(*event_rdpmc),1,fd);
        event_rdpmc = (struct event_log_rdpmc *)(event_t->log_data);

	printf("rdpmc value=%llx\n",event_rdpmc->pmc);	

	fclose(fd);
	free(event_t);
}

void ck_rdseed(void)
{
	event_log_t *event_t;
        struct event_log_rdseed *event_rdseed;
        event_t = (event_log_t *)malloc(sizeof(*event_t)+sizeof(*event_rdseed));
        memset(event_t, 0, sizeof(*event_t)+sizeof(*event_rdseed));
        FILE *fd = fopen("/tmp/event.bin", "r");
        fread(event_t,sizeof(*event_t)+sizeof(*event_rdseed),1,fd);
        event_rdseed = (struct event_log_rdseed *)(event_t->log_data);

        printf("rdseed value=%llx\n",event_rdseed->value);

        fclose(fd);
        free(event_t);
}

void ck_rdtsc(void)
{
        event_log_t *event_t;
        struct event_log_rdtsc *event_rdtsc;
        event_t = (event_log_t *)malloc(sizeof(*event_t)+sizeof(*event_rdtsc));
        memset(event_t, 0, sizeof(*event_t)+sizeof(*event_rdtsc));
        FILE *fd = fopen("/tmp/event.bin", "r");
        fread(event_t,sizeof(*event_t)+sizeof(*event_rdtsc),1,fd);
        event_rdtsc = (struct event_log_rdtsc *)(event_t->log_data);

        printf("rdtsc tsc=%llx\n",event_rdtsc->tsc);

        fclose(fd);
        free(event_t);
}


void ck_rdtscp(void)
{
        event_log_t *event_t;
        struct event_log_rdtscp *event_rdtscp;
        event_t = (event_log_t *)malloc(sizeof(*event_t)+sizeof(*event_rdtscp));
        memset(event_t, 0, sizeof(*event_t)+sizeof(*event_rdtscp));
        FILE *fd = fopen("/tmp/event.bin", "r");
        fread(event_t,sizeof(*event_t)+sizeof(*event_rdtscp),1,fd);
        event_rdtscp = (struct event_log_rdtscp *)(event_t->log_data);

        printf("rdtscp tsc=%llx\n",event_rdtscp->tsc);
        printf("rdtscp tsc_aux=%llx\n",event_rdtscp->tsc_aux);

        fclose(fd);
        free(event_t);
}

void ck_pio(void)
{
}

void ck_mmio(void)
{
        event_log_t *event_t;
        struct event_log_mmio_ins *event_mmio_ins;
        event_t = (event_log_t *)malloc(sizeof(*event_t)+sizeof(*event_mmio_ins));
        memset(event_t, 0, sizeof(*event_t)+sizeof(*event_mmio_ins));
        FILE *fd = fopen("/tmp/event.bin", "r");
        fread(event_t,sizeof(*event_t)+sizeof(*event_mmio_ins),1,fd);
        event_mmio_ins = (struct event_log_mmio_ins *)(event_t->log_data);

        printf("mmio_ins rep=%d\n",event_mmio_ins->rep_num);

        fclose(fd);
        free(event_t);

}


int main(int argc, char *argv[])
{
	int ch;
	char type[BUFFSIZE] = "cpuid";
	while ((ch= getopt(argc, argv, CMD_LINE_ARGS)) != -1) {
		switch (ch) {
                case 't':
			strncpy(type,optarg,sizeof(type));
			break;
		case 'h':
			print_help();
			return 1;
		}
	}
	if (strcmp(type,"cpuid") == 0) {
		ck_cpuid();
	}
	else if (strcmp(type,"rdpmc") == 0) {
		ck_rdpmc();
	}
	else if (strcmp(type,"rdrand") == 0) {
		ck_rdseed();
	}
	else if (strcmp(type,"rdseed") == 0) {
		ck_rdseed();
	}
	else if (strcmp(type,"rdtsc") == 0) {
		ck_rdtsc();
	}
	else if (strcmp(type,"rdtscp") == 0) {
		ck_rdtscp();
	}
	else if (strcmp(type,"pio") == 0) {
		ck_pio();
	}
	else if (strcmp(type,"mmio") == 0) {
		ck_mmio();
	}
	
        return 0;
}

