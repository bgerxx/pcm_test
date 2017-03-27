
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <asm/rcp.h>
//#include <linux/kvm.h>
#include <linux/kvm_types.h>
//#include <linux/kvm_host.h>
#include <asm/kvm_para.h>
#include <linux/smp.h>
#include <linux/init.h>

static void *g_apic = (void *)0xfee00000;
static void *g_ioapic = (void *)0xfec00000;

typedef enum trigger_mode {
    TRIGGER_EDGE = 0,
    TRIGGER_LEVEL,
    TRIGGER_MAX,
} trigger_mode_t;

typedef struct {
    unsigned short offset0;
    unsigned short selector;
    unsigned short ist : 3;
    unsigned short : 5;
    unsigned short type : 4;
    unsigned short : 1;
    unsigned short dpl : 2;
    unsigned short p : 1;
    unsigned short offset1;
#ifdef __x86_64__
    unsigned offset2;
    unsigned reserved;
#endif
} idt_entry_t;

extern idt_entry_t boot_idt[256];
/*extern*/ char isr_entry_point[];

typedef struct {
    uint8_t vector;
    uint8_t delivery_mode:3;
    uint8_t dest_mode:1;
    uint8_t delivery_status:1;
    uint8_t polarity:1;
    uint8_t remote_irr:1;
    uint8_t trig_mode:1;
    uint8_t mask:1;
    uint8_t reserve:7;
    uint8_t reserved[4];
    uint8_t dest_id;
} ioapic_redir_entry_t;

typedef struct {
    ulong regs[sizeof(ulong)*2];
    ulong func;
    ulong rip;
    ulong cs;
    ulong rflags;
} isr_regs_t;

struct apic_ops {
    u32 (*reg_read)(unsigned reg);
    void (*reg_write)(unsigned reg, u32 val);
    void (*icr_write)(u32 val, u32 dest);
    u32 (*id)(void);
};

static inline u16 read_cs(void)
{
    unsigned val;

    asm volatile ("mov %%cs, %0" : "=mr"(val));
    return val;
}

static u32 xapic_read(unsigned reg)
{
    return *(volatile u32 *)(g_apic + reg);
}

static void xapic_write(unsigned reg, u32 val)
{
    *(volatile u32 *)(g_apic + reg) = val;
}

/*static*/ void xapic_icr_write(u32 val, u32 dest)
{
    while (xapic_read(APIC_ICR) & APIC_ICR_BUSY)
        ;
    xapic_write(APIC_ICR2, dest << 24);
    xapic_write(APIC_ICR, val);
}

static uint32_t xapic_id(void)
{
    return xapic_read(APIC_ID) >> 24;
}

static const struct apic_ops xapic_ops = {
    .reg_read = xapic_read,
    .reg_write = xapic_write,
    .icr_write = xapic_icr_write,
    .id = xapic_id,
};

static const struct apic_ops *apic_ops = &xapic_ops;

static u32 x2apic_read(unsigned reg)
{
    unsigned a, d;

    asm volatile ("rdmsr" : "=a"(a), "=d"(d) : "c"(APIC_BASE_MSR + reg/16));
    return a | (u64)d << 32;
}

static void x2apic_write(unsigned reg, u32 val)
{
    asm volatile ("wrmsr" : : "a"(val), "d"(0), "c"(APIC_BASE_MSR + reg/16));
}


/*static*/ void x2apic_icr_write(u32 val, u32 dest)
{
    asm volatile ("wrmsr" : : "a"(val), "d"(dest),
            "c"(APIC_BASE_MSR + APIC_ICR/16));
}

static uint32_t x2apic_id(void)
{
    return x2apic_read(APIC_ID);
}

static const struct apic_ops x2apic_ops = {
    .reg_read = x2apic_read,
    .reg_write = x2apic_write,
    .icr_write = x2apic_icr_write,
    .id = x2apic_id,
};


/*
static inline void outb(uint8_t value, unsigned long port)
{
    asm volatile("outb %b0, %w1" : : "a"(value), "Nd"((unsigned short)port));
}
*/
 void mask_pic_interrupts(void)
{
    outb(0xff, 0x21);
    outb(0xff, 0xa1);
}

static inline void irq_disable(void)
{
    asm volatile("cli");
}

static inline void irq_enable(void)
{
    asm volatile("sti");
}

/*
u32 apic_read(unsigned reg)
{
   return apic_ops->reg_read(reg);
}

void apic_write(unsigned reg, u32 val)
{
   apic_ops->reg_write(reg, val);
}
*/
 bool apic_read_bit(unsigned reg, int n)
{
    reg += (n >> 5) << 4;
    n &= 31;
    return (apic_read(reg) & (1 << n)) != 0;
}


u32 ioapic_read_reg(unsigned reg)
{
    *(volatile u32 *)g_ioapic = reg;
    return *(volatile u32 *)(g_ioapic + 0x10);
}

void ioapic_write_reg(unsigned reg, u32 value)
{
    *(volatile u32 *)g_ioapic = reg;
    *(volatile u32 *)(g_ioapic + 0x10) = value;
}

void ioapic_write_redir(unsigned line, ioapic_redir_entry_t e)
{
    ioapic_write_reg(0x10 + line * 2 + 0, ((u32 *)&e)[0]);
    ioapic_write_reg(0x10 + line * 2 + 1, ((u32 *)&e)[1]);
}

void eoi(void)
{
   apic_write(APIC_EOI, 0);
}

static void set_ioapic_redir(unsigned line, unsigned vec,
			     trigger_mode_t trig_mode)
{
	ioapic_redir_entry_t e = {
		.vector = vec,
		.delivery_mode = 0,
		.trig_mode = trig_mode,
	};

	ioapic_write_redir(line, e);
}

ioapic_redir_entry_t ioapic_read_redir(unsigned line)
{
    ioapic_redir_entry_t e;

    ((u32 *)&e)[0] = ioapic_read_reg(0x10 + line * 2 + 0);
    ((u32 *)&e)[1] = ioapic_read_reg(0x10 + line * 2 + 1);
    return e;

}

void set_mask(unsigned line, int mask)
{
    ioapic_redir_entry_t e = ioapic_read_redir(line);

    e.mask = mask;
    ioapic_write_redir(line, e);
}


static void set_irq_line(unsigned line, int val)
{
	asm volatile("out %0, %1" : : "a"((u8)val), "d"((u16)(0x2000 + line)));
}

static void toggle_irq_line(unsigned line)
{
	set_irq_line(line, 1);
	set_irq_line(line, 0);
}

static void ioapic_reg_version(void)
{
	u8 version_offset;
	uint32_t data_read, data_write;

	version_offset = 0x01;
	data_read = ioapic_read_reg(version_offset);
	data_write = data_read ^ 0xffffffff;

	ioapic_write_reg(version_offset, data_write);
//	report("version register read only test",
//	       data_read == ioapic_read_reg(version_offset));
    printk("version register read only test, version0=%d, version1=%d\n", data_read, ioapic_read_reg(version_offset));
}

static void ioapic_reg_id(void)
{
	u8 id_offset;
	uint32_t data_read, data_write, diff;

	id_offset = 0x0;
	data_read = ioapic_read_reg(id_offset);
	data_write = data_read ^ 0xffffffff;

	ioapic_write_reg(id_offset, data_write);

	diff = data_read ^ ioapic_read_reg(id_offset);
//	report("id register only bits [24:27] writable",
//	       diff == 0x0f000000);
    printk("id register only bits [24:27] writable, diff=%x\n", diff);
}

static void ioapic_arbitration_id(void)
{
	u8 id_offset, arb_offset;
	uint32_t write;

	id_offset = 0x0;
	arb_offset = 0x2;
	write = 0x0f000000;

	ioapic_write_reg(id_offset, write);
//	report("arbitration register set by id",
//	       ioapic_read_reg(arb_offset) == write);
    printk("arbitration register set by id, write=%x, ioapic_read_reg(arb_offset)=%x\n",
            write, ioapic_read_reg(arb_offset));

	ioapic_write_reg(arb_offset, 0x0);
//	report("arbtration register read only",
//               ioapic_read_reg(arb_offset) == write);
    printk("arbtration register read only, write=%x, ioapic_read_reg(arb_offset)=%x\n",
            write, ioapic_read_reg(arb_offset));
}

void set_idt_entry(int vec, void *addr, int dpl)
{
    idt_entry_t *e = &boot_idt[vec];
    memset(e, 0, sizeof *e);
    e->offset0 = (unsigned long)addr;
    e->selector = read_cs();
    e->ist = 0;
    e->type = 14;
    e->dpl = dpl;
    e->p = 1;
    e->offset1 = (unsigned long)addr >> 16;
#ifdef __x86_64__
    e->offset2 = (unsigned long)addr >> 32;
#endif
}

void handle_irq_local(unsigned vec, void (*func)(isr_regs_t *regs))
{
    u8 *thunk = vmalloc(50);

    set_idt_entry(vec, thunk, 0);

#ifdef __x86_64__
    /* sub $8, %rsp */
    *thunk++ = 0x48; *thunk++ = 0x83; *thunk++ = 0xec; *thunk++ = 0x08;
    /* mov $func_low, %(rsp) */
    *thunk++ = 0xc7; *thunk++ = 0x04; *thunk++ = 0x24;
    *(u32 *)thunk = (ulong)func; thunk += 4;
    /* mov $func_high, %(rsp+4) */
    *thunk++ = 0xc7; *thunk++ = 0x44; *thunk++ = 0x24; *thunk++ = 0x04;
    *(u32 *)thunk = (ulong)func >> 32; thunk += 4;
    /* jmp isr_entry_point */
    *thunk ++ = 0xe9;
    *(u32 *)thunk = (ulong)isr_entry_point - (ulong)(thunk + 4);
#else
    /* push $func */
    *thunk++ = 0x68;
    *(u32 *)thunk = (ulong)func; thunk += 4;
    /* jmp isr_entry_point */
    *thunk++ = 0xe9;
    *(u32 *)thunk = (ulong)isr_entry_point - (ulong)(thunk + 4);
#endif
}

static volatile int g_isr_76;

static void ioapic_isr_76(isr_regs_t *regs)
{
	++g_isr_76;
	eoi();
}

static void test_ioapic_edge_intr(void)
{
	handle_irq_local(0x76, ioapic_isr_76);
	set_ioapic_redir(0x0e, 0x76, TRIGGER_EDGE);
	toggle_irq_line(0x0e);
	asm volatile ("nop");
//    report("edge triggered intr", g_isr_76 == 1);
    printk("edge triggered intr, g_isr_76=%x\n", g_isr_76);
}

static volatile int g_isr_77;

static void ioapic_isr_77(isr_regs_t *regs)
{
	++g_isr_77;
	set_irq_line(0x0e, 0);
	eoi();
}

static void test_ioapic_level_intr(void)
{
	handle_irq_local(0x77, ioapic_isr_77);
	set_ioapic_redir(0x0e, 0x77, TRIGGER_LEVEL);
	set_irq_line(0x0e, 1);
	asm volatile ("nop");
//    report("level triggered intr", g_isr_77 == 1);
    printk("level triggered intr, g_isr_77=%x\n", g_isr_77);
}

static int g_78, g_66, g_66_after_78;
static ulong g_66_rip, g_78_rip;

static void ioapic_isr_78(isr_regs_t *regs)
{
	++g_78;
	g_78_rip = regs->rip;
	eoi();
}

static void ioapic_isr_66(isr_regs_t *regs)
{
	++g_66;
	if (g_78)
		++g_66_after_78;
	g_66_rip = regs->rip;
	eoi();
}

static void test_ioapic_simultaneous(void)
{
	handle_irq_local(0x78, ioapic_isr_78);
	handle_irq_local(0x66, ioapic_isr_66);
	set_ioapic_redir(0x0e, 0x78, TRIGGER_EDGE);
	set_ioapic_redir(0x0f, 0x66, TRIGGER_EDGE);
	irq_disable();
	toggle_irq_line(0x0f);
	toggle_irq_line(0x0e);
	irq_enable();
	asm volatile ("nop");
//    report("ioapic simultaneous edge interrupts",
//            g_66 && g_78 && g_66_after_78 && g_66_rip == g_78_rip);
    printk("ioapic simultaneous edge interrupts, g_66 && g_78 && g_66_after_78 && g_66_rip=Dec%d, g_78_rip=Dec%ld\n",
            g_66 && g_78 && g_66_after_78 && g_66_rip, g_78_rip);
}

static volatile int g_tmr_79 = -1;

static void ioapic_isr_79(isr_regs_t *regs)
{
	g_tmr_79 = apic_read_bit(APIC_TMR, 0x79);
	set_irq_line(0x0e, 0);
	eoi();
}

static void test_ioapic_edge_tmr(bool expected_tmr_before)
{
	int tmr_before;

	handle_irq_local(0x79, ioapic_isr_79);
	set_ioapic_redir(0x0e, 0x79, TRIGGER_EDGE);
	tmr_before = apic_read_bit(APIC_TMR, 0x79);
	toggle_irq_line(0x0e);
	asm volatile ("nop");
//    report("TMR for ioapic edge interrupts (expected %s)",
//            tmr_before == expected_tmr_before && !g_tmr_79,
//            expected_tmr_before ? "true" : "false");
    printk("TMR for ioapic edge interrupts, tmr_before=%x, expected_tmr_before=%x, g_tmr_79=%x\n",
            tmr_before, expected_tmr_before, g_tmr_79);
}

static void test_ioapic_level_tmr(bool expected_tmr_before)
{
	int tmr_before;

	handle_irq_local(0x79, ioapic_isr_79);
	set_ioapic_redir(0x0e, 0x79, TRIGGER_LEVEL);
	tmr_before = apic_read_bit(APIC_TMR, 0x79);
	set_irq_line(0x0e, 1);
	asm volatile ("nop");
//	report("TMR for ioapic level interrupts (expected %s)",
//	       tmr_before == expected_tmr_before && g_tmr_79,
//	       expected_tmr_before ? "true" : "false");
    printk("TMR for ioapic level interrupts, tmr_before=%x, expected_tmr_before=%x, g_tmr_79=%x\n",
            tmr_before, expected_tmr_before, g_tmr_79);
}

#define IPI_DELAY 1000000

static void delay(int count)
{
	while(count--) asm("");
}

static void toggle_irq_line_0x0e(void *data)
{
	irq_disable();
	delay(IPI_DELAY);
	toggle_irq_line(0x0e);
	irq_enable();
}

static void test_ioapic_edge_tmr_smp(bool expected_tmr_before)
{
	int tmr_before;
	int i;

	g_tmr_79 = -1;
	handle_irq_local(0x79, ioapic_isr_79);
	set_ioapic_redir(0x0e, 0x79, TRIGGER_EDGE);
	tmr_before = apic_read_bit(APIC_TMR, 0x79);
//	on_cpu_async(1, toggle_irq_line_0x0e, 0);
    smp_call_function_single(1, toggle_irq_line_0x0e, NULL, 0);
	i = 0;
	while(g_tmr_79 == -1) i++;
	printk("%d iterations before interrupt received\n", i);
//	report("TMR for ioapic edge interrupts (expected %s)",
//	       tmr_before == expected_tmr_before && !g_tmr_79,
//	       expected_tmr_before ? "true" : "false");
    printk("TMR for ioapic edge interrupts, tmr_before=%x, expected_tmr_before=%x, g_tmr_79=%x\n",
            tmr_before, expected_tmr_before, g_tmr_79);
}

static void set_irq_line_0x0e(void *data)
{
	irq_disable();
	delay(IPI_DELAY);
	set_irq_line(0x0e, 1);
	irq_enable();
}

static void test_ioapic_level_tmr_smp(bool expected_tmr_before)
{
	int i, tmr_before;

	g_tmr_79 = -1;
	handle_irq_local(0x79, ioapic_isr_79);
	set_ioapic_redir(0x0e, 0x79, TRIGGER_LEVEL);
	tmr_before = apic_read_bit(APIC_TMR, 0x79);
//	on_cpu_async(1, set_irq_line_0x0e, 0);
    smp_call_function_single(1, set_irq_line_0x0e, NULL, 0);
	i = 0;
	while(g_tmr_79 == -1) i++;
	printk("%d iterations before interrupt received\n", i);
//	report("TMR for ioapic level interrupts (expected %s)",
//	       tmr_before == expected_tmr_before && g_tmr_79,
//	       expected_tmr_before ? "true" : "false");
    printk("TMR for ioapic level interrupts tmr_before=%x, expected_tmr_before=%x, g_tmr_79=%x\n",
            tmr_before, expected_tmr_before, g_tmr_79);
}

static int g_isr_98;

static void ioapic_isr_98(isr_regs_t *regs)
{
	++g_isr_98;
	if (g_isr_98 == 1) {
		set_irq_line(0x0e, 0);
		set_irq_line(0x0e, 1);
	}
	set_irq_line(0x0e, 0);
	eoi();
}

static void test_ioapic_level_coalesce(void)
{
	handle_irq_local(0x98, ioapic_isr_98);
	set_ioapic_redir(0x0e, 0x98, TRIGGER_LEVEL);
	set_irq_line(0x0e, 1);
	asm volatile ("nop");
	printk("coalesce simultaneous level interrupts g_isr_98=%x,\n", g_isr_98);
}

static int g_isr_99;

static void ioapic_isr_99(isr_regs_t *regs)
{
	++g_isr_99;
	set_irq_line(0x0e, 0);
	eoi();
}

static void test_ioapic_level_sequential(void)
{
	handle_irq_local(0x99, ioapic_isr_99);
	set_ioapic_redir(0x0e, 0x99, TRIGGER_LEVEL);
	set_irq_line(0x0e, 1);
	set_irq_line(0x0e, 1);
	asm volatile ("nop");
	printk("sequential level interrupts g_isr_99=%x\n", g_isr_99);
}

static volatile int g_isr_9a;

static void ioapic_isr_9a(isr_regs_t *regs)
{
	++g_isr_9a;
	if (g_isr_9a == 2)
		set_irq_line(0x0e, 0);
	eoi();
}

static void test_ioapic_level_retrigger(void)
{
	int i;

	handle_irq_local(0x9a, ioapic_isr_9a);
	set_ioapic_redir(0x0e, 0x9a, TRIGGER_LEVEL);

	asm volatile ("cli");
	set_irq_line(0x0e, 1);

	for (i = 0; i < 10; i++) {
		if (g_isr_9a == 2)
			break;

		asm volatile ("sti; hlt; cli");
	}

	asm volatile ("sti");

	printk("retriggered level interrupts without masking g_isr_9a=%x\n", g_isr_9a);
}

static volatile int g_isr_81;

static void ioapic_isr_81(isr_regs_t *regs)
{
	++g_isr_81;
	set_irq_line(0x0e, 0);
	eoi();
}

static void test_ioapic_edge_mask(void)
{
	handle_irq_local(0x81, ioapic_isr_81);
	set_ioapic_redir(0x0e, 0x81, TRIGGER_EDGE);

	set_mask(0x0e, true);
	set_irq_line(0x0e, 1);
	set_irq_line(0x0e, 0);

	asm volatile ("nop");
	printk("masked level interrupt g_isr_81=%x\n", g_isr_81);

	set_mask(0x0e, false);
	set_irq_line(0x0e, 1);

	asm volatile ("nop");
	printk("unmasked level interrupt g_isr_81=%x\n", g_isr_81);
}

static volatile int g_isr_82;

static void ioapic_isr_82(isr_regs_t *regs)
{
	++g_isr_82;
	set_irq_line(0x0e, 0);
	eoi();
}

static void test_ioapic_level_mask(void)
{
	handle_irq_local(0x82, ioapic_isr_82);
	set_ioapic_redir(0x0e, 0x82, TRIGGER_LEVEL);

	set_mask(0x0e, true);
	set_irq_line(0x0e, 1);

	asm volatile ("nop");
	printk("masked level interrupt g_isr_82=%x\n", g_isr_82);

	set_mask(0x0e, false);

	asm volatile ("nop");
	printk("unmasked level interrupt g_isr_82=%x\n", g_isr_82);
}

static volatile int g_isr_83;

static void ioapic_isr_83(isr_regs_t *regs)
{
	++g_isr_83;
	set_mask(0x0e, true);
	eoi();
}

static void test_ioapic_level_retrigger_mask(void)
{
	handle_irq_local(0x83, ioapic_isr_83);
	set_ioapic_redir(0x0e, 0x83, TRIGGER_LEVEL);

	set_irq_line(0x0e, 1);
	asm volatile ("nop");
	set_mask(0x0e, false);
	asm volatile ("nop");
	printk("retriggered level interrupts with mask g_isr_83=%x\n", g_isr_83);

	set_irq_line(0x0e, 0);
	set_mask(0x0e, false);
}

int enable_x2apic_local(void)
{
    unsigned a, b, c, d;

    asm ("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(1));

    if (c & (1 << 21)) {
        asm ("rdmsr" : "=a"(a), "=d"(d) : "c"(MSR_IA32_APICBASE));
        a |= 1 << 10;
        asm ("wrmsr" : : "a"(a), "d"(d), "c"(MSR_IA32_APICBASE));
        apic_ops = &x2apic_ops;
        return 1;
    } else {
        return 0;
    }
}

void test_ioapic(void)
{
//	setup_vm();
//	smp_init();

	mask_pic_interrupts();

    
	if (enable_x2apic_local() != 0)
		printk("x2apic enabled\n");
	else
		printk("x2apic not detected\n");

//    enable_x2apic();

	irq_enable();

	ioapic_reg_version();
	ioapic_reg_id();
	ioapic_arbitration_id();

	test_ioapic_edge_intr();
	test_ioapic_level_intr();
	test_ioapic_simultaneous();

	test_ioapic_level_coalesce();
	test_ioapic_level_sequential();
	test_ioapic_level_retrigger();

	test_ioapic_edge_mask();
	test_ioapic_level_mask();
	test_ioapic_level_retrigger_mask();

	test_ioapic_edge_tmr(false);
	test_ioapic_level_tmr(false);
	test_ioapic_level_tmr(true);
	test_ioapic_edge_tmr(true);

	if (/*cpu_count()*/num_online_cpus() > 1) {
		test_ioapic_edge_tmr_smp(false);
		test_ioapic_level_tmr_smp(false);
		test_ioapic_level_tmr_smp(true);
		test_ioapic_edge_tmr_smp(true);
	}

}
