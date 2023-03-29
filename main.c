#include "malloc.h"
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    malloc(16);

    // printf("s: [%s]\n", s);
    show_alloc_mem_info();
    // // printf("s: [%p]\n", s);

    char *s = malloc(10000);

    show_alloc_mem_info();

    realloc(s, 524212);

    // printf("s: [%s]\n", s);
    show_alloc_mem_info();
    // printf("s: [%p]\n", s);
    return (0);
}