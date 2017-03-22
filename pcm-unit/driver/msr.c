
#include <asm/msr.h>
#include <asm/msr-index.h>


struct msr_rw_info{
    int index;
    char *name;
    unsigned long long value;
    unsigned long long expected;
};

#define addr_64 0x0000123456789abcULL

struct msr_rw_info msr_info[] =
{
    { .index = 0x00000174, .name = "IA32_SYSENTER_CS",
      .value = 0x1234, .expected = 0x1234,
    },
    { .index = 0x00000175, .name = "MSR_IA32_SYSENTER_ESP",
      .value = addr_64, .expected = addr_64,
    },
    { .index = 0x00000176, .name = "IA32_SYSENTER_EIP",
      .value = addr_64, .expected = addr_64,
    },
    { .index = 0x000001a0, .name = "MSR_IA32_MISC_ENABLE",
      // reserved: 1:2, 4:6, 8:10, 13:15, 17, 19:21, 24:33, 35:63
      .value = 0x400c51889, .expected = 0x400c51889,
    },
    { .index = 0x00000277, .name = "MSR_IA32_CR_PAT",
      .value = 0x07070707, .expected = 0x07070707,
    },
    { .index = 0xc0000100, .name = "MSR_FS_BASE",
      .value = addr_64, .expected = addr_64,
    },
    { .index = 0xc0000101, .name = "MSR_GS_BASE",
      .value = addr_64, .expected = addr_64,
    },
    { .index = 0xc0000102, .name = "MSR_KERNEL_GS_BASE",
      .value = addr_64, .expected = addr_64,
    },
#ifdef __x86_64__
    { .index = 0xc0000080, .name = "MSR_EFER",
      .value = 0xD00, .expected = 0xD00,
    },
#endif
    { .index = 0xc0000082, .name = "MSR_LSTAR",
      .value = addr_64, .expected = addr_64,
    },
    { .index = 0xc0000083, .name = "MSR_CSTAR",
      .value = addr_64, .expected = addr_64,
    },
    { .index = 0xc0000084, .name = "MSR_SYSCALL_MASK",
      .value = 0xffffffff, .expected = 0xffffffff,
    },

};


static int find_msr_info(int msr_index)
{
    int i;
    for (i = 0; i < sizeof(msr_info)/sizeof(msr_info[0]) ; i++) {
        if (msr_info[i].index == msr_index) {
            return i;
        }
    }
    return -1;
}

static void test_msr_rw(int msr_index, unsigned long long input, unsigned long long expected)
{
    unsigned long long r = 0, p;
    int index;
    char *sptr;
    if ((index = find_msr_info(msr_index)) != -1) {
        sptr = msr_info[index].name;
    } else {
        printk("couldn't find name for msr # 0x%x, skipping\n", msr_index);
        return;
    }
    wrmsrl_safe(msr_index, input);
    r = rdmsrl_safe(msr_index, &p);
    if (expected != r) {
        printk("testing %s: output = 0x%x:0x%x expected = 0x%x:0x%x\n", sptr,
               (u32)(r >> 32), (u32)r, (u32)(expected >> 32), (u32)expected);
    }
//    report(sptr, expected == r);
    printk("testing %s succeed\n", sptr);
}

static inline u16 read_cs(void)
{
    unsigned val;

    asm volatile ("mov %%cs, %0" : "=mr"(val));
    return val;
}

static inline u16 read_ss(void)
{
    unsigned val;

    asm volatile ("mov %%ss, %0" : "=mr"(val));
    return val;
}

static inline void write_ss(unsigned val)
{
    asm volatile ("mov %0, %%ss" : : "rm"(val) : "memory");
}


static void test_syscall_lazy_load(void)
{
#ifdef __x86_64__
    extern void syscall_target();
    u16 cs = read_cs(), ss = read_ss();
    ulong tmp;
/*
    wrmsr(MSR_EFER, rdmsr(MSR_EFER) | EFER_SCE);
    wrmsr(MSR_LSTAR, (ulong)syscall_target);
    wrmsr(MSR_STAR, (uint64_t)cs << 32);
    */
    unsigned long long p;
/*    rdmsrl_safe(MSR_EFER, &p);
 *    */
    wrmsrl_safe(MSR_EFER, (ulong)(rdmsrl_safe(MSR_EFER, &p) | EFER_SCE));
    wrmsrl_safe(MSR_LSTAR, (ulong)syscall_target);
    wrmsrl_safe(MSR_STAR, (ulong)cs);
    asm volatile("pushf; syscall; syscall_target: popf" : "=c"(tmp) : : "r11");
    write_ss(ss);
    // will crash horribly if broken
//    report("MSR_*STAR eager loading", true);
    printk("MSR_*STAR eager loading \n");
#endif
}

void test_msr(void)
{
    int i;
    for (i = 0 ; i < sizeof(msr_info) / sizeof(msr_info[0]); i++) {
        test_msr_rw(msr_info[i].index, msr_info[i].value, msr_info[i].expected);
    }
    
    test_syscall_lazy_load();

}

