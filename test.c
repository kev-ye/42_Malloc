#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

int main() {
    int n = 0;

    int a = 123456;
    int* p = &a;

    printf("n: %d\n", n);
    printf("a: %d\n", a);
    printf("pn: %p\n", &n);
    printf("pa: %p\n", p);

    memcpy(&n, p, sizeof(int));

    printf("\nn: %d\n", n);
    printf("a: %d\n", a);
    printf("pn: %p\n", &n);
    printf("pa: %p\n", p);
}