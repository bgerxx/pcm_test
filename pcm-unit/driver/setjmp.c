
//#include <longjmp.h>
//#include <setjmp.h>

#include "linux/kernel.h"
#include "test_func.h"


typedef struct jmp_buf_tag {
    long int regs[8];
} jmp_buf[1];

extern int setjmp (struct jmp_buf_tag env[1]);
extern void longjmp (struct jmp_buf_tag env[1], int val)
     __attribute__ ((__noreturn__));



void test_setjmp(void)
{
    volatile int i;
    jmp_buf j;

    if (setjmp(j) == 0)
    {
        i = 0;
    }
    printk("%d\n", i);

    if (++i < 10)
    {
        longjmp(j, 1);
    }

    printk("done\n");
}
