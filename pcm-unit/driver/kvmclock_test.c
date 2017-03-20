
#include <linux/kernel.h>
#include <linux/module.h>
#include "test_func.h"
#include <asm/rcp.h>
//#include <asm/kvm_host.h>
//#include <asm/pvclock-abi.h>
//#include <linux/kvm.h>
#include <linux/kvm_types.h>
#include <linux/clocksource.h>
//#include <linux/kvm_host.h>
#include <asm/pvclock.h>
#include <asm/kvm_para.h>
#include <linux/smp.h>
#include <asm/atomic.h>
#include <../../kernel/smpboot.h>
#include <linux/init.h>

#define DEFAULT_TEST_LOOPS 100000000L
#define DEFAULT_THRESHOLD  5L

#define PVCLOCK_RAW_CYCLE_BIT (1 << 7) /* Get raw cycle */

#define MAX_CPU 64

//#define PRIu64  __PRI64_PREFIX  "u"

long loops = DEFAULT_TEST_LOOPS;
long sec = 0;
long threshold = DEFAULT_THRESHOLD;

struct test_info {
        struct spinlock lock;
        u64 warps;                /* warp count */
        u64 stalls;               /* stall count */
        long long worst;          /* worst warp */
        volatile cycle_t last;    /* last cycle seen by test */
        atomic_t ncpus;           /* number of cpu in the test*/
        int check;                /* check cycle ? */
};

struct test_info ti[4];

struct pvclock_vcpu_time_info hv_clock[MAX_CPU];

//extern cycle_t pvclock_clocksource_read(struct pvclock_vcpu_time_info *src);
//extern void kvm_get_wallclock(struct timespec *now);
//extern cycle_t kvm_clock_read(void);
//extern int kvm_register_clock(char *txt);
//extern void /*__init*/ smp_init(void);

static u8 valid_flags __read_mostly = 0;
static atomic64_t last_value = ATOMIC64_INIT(0);
struct pvclock_wall_clock wall_clock;

void pvclock_set_flags(u8 flags)
{
   valid_flags = flags;
}


int smp_id(void)
{
//    unsigned id;

//    asm ("mov %%gs:0, %0" : "=r"(id));
//    return id;
    return smp_processor_id(); 
}

/*
static void setup_smp_id(void *data)
{
   asm ("mov %0, %%gs:0" : : "r"(apic_id()) : "memory");
}
*/

//extern int cpu_up(unsigned int cpu);
//void __init smp_init(void)
//{
//    unsigned int cpu;
//
//    idle_threads_init();
//
//    /* FIXME: This should be done in userspace --RR */
//    for_each_present_cpu(cpu) {
//        if (num_online_cpus() >= setup_max_cpus)
//            break;
////        if (!cpu_online(cpu))
////           smp_call_function_single(cpu, setup_smp_id, (void *)0, 0);
////            cpu_up(cpu);
//    }
//
//    /* Any cleanup work */
//    printk(KERN_INFO "Brought up %ld CPUs\n", (long)num_online_cpus());
//    smp_cpus_done(setup_max_cpus);
//}

void pvclock_read_wallclock(struct pvclock_wall_clock *wall_clock,
        struct pvclock_vcpu_time_info *vcpu_time,
        struct timespec *ts)
{
    u32 version;
    u64 delta;
    struct timespec now;

    /* get wallclock at system boot */
    do {
        version = wall_clock->version;
        rmb();      /* fetch version before time */
        now.tv_sec  = wall_clock->sec;
        now.tv_nsec = wall_clock->nsec;
        rmb();      /* fetch time before checking version */
    } while ((wall_clock->version & 1) || (version != wall_clock->version));

    delta = pvclock_clocksource_read(vcpu_time);    /* time since system boot */
    delta += now.tv_sec * (u64)NSEC_PER_SEC + now.tv_nsec;

    now.tv_nsec = do_div(delta, NSEC_PER_SEC);
    now.tv_sec = delta;

    set_normalized_timespec(ts, now.tv_sec, now.tv_nsec);
}


cycle_t pvclock_clocksource_read(struct pvclock_vcpu_time_info *src)
{
    unsigned version;
    cycle_t ret;
    u64 last;
    u8 flags;

/***************************/
    do {
        version = __pvclock_read_cycles(src, &ret, &flags);
    } while ((src->version & 1) || version != src->version);
/*
    if (unlikely((flags & PVCLOCK_GUEST_STOPPED) != 0)) {
        src->flags &= ~PVCLOCK_GUEST_STOPPED;
        pvclock_touch_watchdogs();
    }
*/

    if ((valid_flags & PVCLOCK_TSC_STABLE_BIT) &&
            (flags & PVCLOCK_TSC_STABLE_BIT))
        return ret;


/***************************/    
/*    
    do {
        version = pvclock_read_begin(src);
        ret = __pvclock_read_cycles(src);
        flags = src->flags;
    } while (pvclock_read_retry(src, version));

    if ((valid_flags & PVCLOCK_RAW_CYCLE_BIT) ||
            ((valid_flags & PVCLOCK_TSC_STABLE_BIT) &&
             (flags & PVCLOCK_TSC_STABLE_BIT)))
        return ret;
*/

    last = atomic64_read(&last_value);
    do {
        if (ret < last)
            return last;
        last = atomic64_cmpxchg(&last_value, last, ret);
    } while (unlikely(last != ret));

    return ret;
}

void kvm_get_wallclock(struct timespec *ts)
{
    struct pvclock_vcpu_time_info *vcpu_time;
    int index = smp_id();

    wrmsrl_safe(MSR_KVM_WALL_CLOCK_NEW, (unsigned long)&wall_clock);
    vcpu_time = &hv_clock[index];
    pvclock_read_wallclock(&wall_clock, vcpu_time, ts);
}


cycle_t kvm_clock_read(void)
{
    struct pvclock_vcpu_time_info *src;
    cycle_t ret;
    int index = smp_id();

    src = &hv_clock[index];
    ret = pvclock_clocksource_read(src);
    return ret;
}


static void wallclock_test(void *data)
{
        int *p_err = data;
        long ksec, offset;
        struct timespec ts;

        kvm_get_wallclock(&ts);
        ksec = ts.tv_sec;

        offset = ksec - sec;
        printk("Raw nanoseconds value from kvmclock: %llu (cpu %d)\n", kvm_clock_read(), smp_id());
        printk("Seconds get from kvmclock: %ld (cpu %d, offset: %ld)\n", ksec, smp_id(), offset);

        if (offset > threshold || offset < -threshold) {
                printk("offset too large!\n");
                (*p_err)++;
        }
}

/*
static void kvm_clock_init(void *data)
{
//   kvmclock_init(); 
    kvm_register_clock(data);
}
*/

void kvm_clock_init(void *data)
{
    int index = smp_id();
    struct pvclock_vcpu_time_info *hvc = &hv_clock[index];

    printk("kvm-clock: cpu %d, msr %p\n", index, hvc);
    wrmsrl_safe(MSR_KVM_SYSTEM_TIME_NEW, (unsigned long)hvc | 1);
}

void kvm_clock_clear(void *data)
{
    wrmsrl_safe(MSR_KVM_SYSTEM_TIME_NEW, 0LL);
}


static void kvm_clock_test(void *data)
{
        struct test_info *hv_test_info = (struct test_info *)data;
        long i, check = hv_test_info->check;

        for (i = 0; i < loops; i++){
                cycle_t t0, t1;
                long long delta;

                if (check == 0) {
                        kvm_clock_read();
                        continue;
                }

                spin_lock(&hv_test_info->lock);
                t1 = kvm_clock_read();
                t0 = hv_test_info->last;
                hv_test_info->last = kvm_clock_read();
                spin_unlock(&hv_test_info->lock);

                delta = t1 - t0;
                if (delta < 0) {
                        spin_lock(&hv_test_info->lock);
                        ++hv_test_info->warps;
                        if (delta < hv_test_info->worst){
                                hv_test_info->worst = delta;
                                printk("Worst warp %lld\n", hv_test_info->worst);
                        }
                        spin_unlock(&hv_test_info->lock);
                }
                if (delta == 0)
                        ++hv_test_info->stalls;

                if (!((unsigned long)i & 31))
                        asm volatile("rep; nop");
        }

        atomic_dec(&hv_test_info->ncpus);
}

static int cycle_test(int ncpus, int check, struct test_info *ti)
{
        int i;
        unsigned long long begin, end;

//        begin = rdtsc();
        rdtscll(begin);        

        atomic_set(&ti->ncpus, ncpus);
        ti->check = check;
        for (i = ncpus - 1; i >= 0; i--)
                smp_call_function_single(i, kvm_clock_test, (void *)ti, 0);

        /* Wait for the end of other vcpu */
        while(atomic_read(&ti->ncpus))
                ;

/*        
        end = rdtsc();*/
        rdtscll(end);

        printk("Total vcpus: %d\n", ncpus);
        printk("Test  loops: %ld\n", loops);
        if (check == 1) {
                printk("Total warps:  %lld\n", ti->warps);
                printk("Total stalls: %lld\n", ti->stalls);
                printk("Worst warp:   %lld\n", ti->worst);
        } else
                printk("TSC cycles:  %lld\n", end - begin);

        return ti->warps ? 1 : 0;
}

//int test_kvm_clock(int ac, char **av)
void test_kvm_clock(void)
{
        int nerr = 0;
        int ncpus;//, maxcpus;
        int i;

/*
        if (ac > 1)
                loops = atol(av[1]);
        if (ac > 2)
                sec = atol(av[2]);
        if (ac > 3)
                threshold = atol(av[3]);
*/
        sec = 2;

//        smp_init();

//        ncpus = cpu_count();
        ncpus = num_online_cpus();
       // maxcpus = setup_max_cpus;

        if (ncpus > MAX_CPU)
                ncpus = MAX_CPU;

/*        
        if (ncpus > maxcpus)
            ncpus = maxcpus;
*/            
        for (i = 0; i < ncpus; ++i)
                smp_call_function_single(i, kvm_clock_init, (void *)0, 1);

//        if (ac > 2) {
                printk("Wallclock test, threshold %ld\n", threshold);
                printk("Seconds get from host:     %ld\n", sec);
                for (i = 0; i < ncpus; ++i)
                    smp_call_function_single(i, wallclock_test, &nerr, 1);
//                        on_cpu(i, wallclock_test, &nerr);
//        } here maybe should use the kernel's protogenic api: smp_init();

        printk("Check the stability of raw cycle ...\n");
        pvclock_set_flags(PVCLOCK_TSC_STABLE_BIT
                          | PVCLOCK_RAW_CYCLE_BIT);
        if (cycle_test(ncpus, 1, &ti[0]))
                printk("Raw cycle is not stable\n");
        else
                printk("Raw cycle is stable\n");

        pvclock_set_flags(PVCLOCK_TSC_STABLE_BIT);
        printk("Monotonic cycle test:\n");
        nerr += cycle_test(ncpus, 1, &ti[1]);

        printk("Measure the performance of raw cycle ...\n");
        pvclock_set_flags(PVCLOCK_TSC_STABLE_BIT
                          | PVCLOCK_RAW_CYCLE_BIT);
        cycle_test(ncpus, 0, &ti[2]);

        printk("Measure the performance of adjusted cycle ...\n");
        pvclock_set_flags(PVCLOCK_TSC_STABLE_BIT);
        cycle_test(ncpus, 0, &ti[3]);

        for (i = 0; i < ncpus; ++i)
                smp_call_function_single(i, kvm_clock_clear, (void *)0, 1);

//        return nerr > 0 ? 1 : 0;
}
