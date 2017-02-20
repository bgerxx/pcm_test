#include<stdio.h>
typedef unsigned long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#pragma pack(push)
#pragma pack(1)
struct pcm_cpu_msrs {
       uint64_t msr_kernel_gs_base;
       uint64_t msr_effer;
};

struct pcm_cpu_user_regs {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rax;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rip;
    uint16_t cs, _pad0[3];
    uint32_t cs_limit, _pad1;
    uint32_t cs_flags, _pad2;
    uint64_t cs_base;
    uint64_t rflags;
    uint64_t rsp;
    uint16_t ss, _pad3[3];
    uint32_t ss_limit,_pad4;
    uint32_t ss_flags,_pad5;
    uint64_t ss_base;
    uint16_t es, _pad6[3];
    uint32_t es_limit,_pad7;
    uint32_t es_flags,_pad8;
    uint64_t es_base;
    uint16_t ds, _pad9[3];
    uint32_t ds_limit,_pad10;
    uint32_t ds_flags,_pad11;
    uint64_t ds_base;
    uint16_t fs, _pad12[3];
    uint32_t fs_limit,_pad13;
    uint32_t fs_flags,_pad14;
    uint64_t fs_base;
    uint16_t gs, _pad15[3];
    uint32_t gs_limit,_pad16;
    uint32_t gs_flags,_pad17;
    uint64_t gs_base;
};

struct  pcm_cpu_non_reg_states {
    uint32_t activity;
    uint32_t interruptibility;
    uint64_t pending_debug_exceptions;
    uint64_t vmx_preemption_time;
    uint64_t vmcs_link; // may not use if no virtual VMX
    uint64_t pdpte[4];  // may not use if no virtual EPT
};

struct pcm_cpu_seg_regs {
    uint64_t gdtr_base;
    uint32_t gdtr_limit, _pad0;
    uint64_t idtr_base;
    uint32_t idtr_limit, _pad1;
    uint64_t ldtr_base;
    uint32_t ldtr_limit;
    uint16_t ldtr_ss, pad2;
    uint64_t tr_base;
    uint32_t tr_limit;
    uint16_t tr_ss, pad3;
};

struct pcm_cpu_cr_regs {
    uint64_t cr0, cr2, cr3, cr4, cr8, xcr0;
};

struct pcm_cpu_dr_regs {
    uint64_t dr0, dr1, dr2, dr3, dr6, dr7;
};

struct pcm_cpu_states {
    struct pcm_cpu_user_regs uregs;
    struct pcm_cpu_non_reg_states nregs;
    struct pcm_cpu_seg_regs sregs;
    struct pcm_cpu_cr_regs  crs;
    struct pcm_cpu_dr_regs  drs;
    struct pcm_cpu_msrs  msrs;
    uint64_t apic;
    uint64_t p_msrs;
    uint64_t pmu;
    uint64_t fpu;

    uint64_t platform_info;
    uint64_t eptp;
};

#pragma pack(pop)
int main(int argc, char*argv[])
{
	struct pcm_cpu_states p1;
	FILE *fd1 = fopen("/tmp/final_cpu.bin", "r");
	fread(&p1, sizeof(p1), 1, fd1); 
	printf("====pcm_cpu_user_regs====\n");
	printf("r15: %lu\n",p1.uregs.r15);
	printf("r14: %lu\n",p1.uregs.r14);
	printf("r13: %lu\n",p1.uregs.r13);
	printf("r12: %lu\n",p1.uregs.r12);
	printf("rbp: %lu\n",p1.uregs.rbp);
	printf("rbx: 0x%llx\n",p1.uregs.rbx);
	printf("r11: %lu\n",p1.uregs.r11);
	printf("r10: %lu\n",p1.uregs.r10);
	printf("r9: %lu\n",p1.uregs.r9);
	printf("r8: %lu\n",p1.uregs.r8);
	printf("rax: 0x%llx\n",p1.uregs.rax);
	printf("rcx: 0x%llx\n",p1.uregs.rcx);
	printf("rdx: 0x%llx\n",p1.uregs.rdx);
	printf("rsi: %lu\n",p1.uregs.rsi);
	printf("rdi: %lu\n",p1.uregs.rdi);
	printf("rip: %lu\n",p1.uregs.rip);
	printf("cs: %d\n",p1.uregs.cs);
	printf("cs_limit: %u\n",p1.uregs.cs_limit);
	printf("cs_flags: %u\n",p1.uregs.cs_flags);
	printf("cs_base: %lu\n",p1.uregs.cs_base);
	printf("ds: %d\n",p1.uregs.ds);
	printf("ds_limit: %u\n",p1.uregs.ds_limit);
	printf("ds_flags: %u\n",p1.uregs.ds_flags);
	printf("ds_base: %lu\n",p1.uregs.ds_base);
	printf("ss: %d\n",p1.uregs.ss);
	printf("ss_limit: %u\n",p1.uregs.ss_limit);
	printf("ss_flags: %u\n",p1.uregs.ss_flags);
	printf("ss_base: %lu\n",p1.uregs.ss_base);
	printf("es: %d\n",p1.uregs.es);
	printf("es_limit: %u\n",p1.uregs.es_limit);
	printf("es_flags: %u\n",p1.uregs.es_flags);
	printf("es_base: %lu\n",p1.uregs.es_base);
	printf("fs: %d\n",p1.uregs.fs);
	printf("fs_limit: %u\n",p1.uregs.fs_limit);
	printf("fs_flags: %u\n",p1.uregs.fs_flags);
	printf("fs_base: %lu\n",p1.uregs.fs_base);
	printf("gs: %d\n",p1.uregs.gs);
	printf("gs_limit: %u\n",p1.uregs.gs_limit);
	printf("gs_flags: %u\n",p1.uregs.gs_flags);
	printf("gs_base: %lu\n",p1.uregs.gs_base);
	printf("rflags: %lu\n",p1.uregs.rflags);
	printf("rsp: %lu\n",p1.uregs.rsp);

	printf("====pcm_cpu_cr_regs====\n");
	printf("cr0: %lu\n",p1.crs.cr0);
	printf("cr2: %lu\n",p1.crs.cr2);
	printf("cr3: %lu\n",p1.crs.cr3);
	printf("cr4: %lu\n",p1.crs.cr4);
	printf("cr8: %lu\n",p1.crs.cr8);
	printf("xcr0: %lu\n",p1.crs.xcr0);

	printf("====pcm_cpu_dr_regs====\n");
	printf("dr0: %lu\n",p1.drs.dr0);
	printf("dr1: %lu\n",p1.drs.dr1);
	printf("dr2: %lu\n",p1.drs.dr2);
	printf("dr3: %lu\n",p1.drs.dr3);
	printf("dr6: %lu\n",p1.drs.dr6);
	printf("dr7: %lu\n",p1.drs.dr7);

	printf("====pcm_cpu_msrs====\n");
	printf("msr_kernel_gs_base: %lu\n",p1.msrs.msr_kernel_gs_base);
	printf("msr_effer: %lu\n",p1.msrs.msr_effer);
	
	return 0;
}
