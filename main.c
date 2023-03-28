#include "malloc.h"
// #include <stdlib.h>
#include <stdio.h>

int main()
{
    __unused
    char *s = malloc(16);

    __unused
    char a = 'a';
    for (int i = 0; i < 16; i++)
        s[i] = a++;
    s[15] = '\0';

    // printf("s: [%s]\n", s);
    show_alloc_mem_info();
    // // printf("s: [%p]\n", s);

    s = malloc(10000);

    show_alloc_mem_info();

    s = realloc(s, 524212);

    // printf("s: [%s]\n", s);
    show_alloc_mem_info();
    // printf("s: [%p]\n", s);

    return (0);
}