/*
 * Please use gcc > 5.1
 * gcc -fcheck-pointer-bounds -mmpx focus_mpx.c
 *
 */

#include <stdlib.h>
int main(int argc, char **argv) {
    char *x = (char*)malloc(argc * 10);
    x[argc + 10] = 0;
    int res = x[10];
    free(x);
    return res;
}
