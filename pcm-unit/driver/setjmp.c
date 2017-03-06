
#include <longjmp.h>

#include "test_func.h"


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
