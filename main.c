#include "malloc.h"
// #include <stdlib.h>

int main()
{
    __unused
    char *s = ft_malloc(4000);

    show_alloc_mem_info();

    __unused
    char *s2 = ft_realloc(s, 2000);

    show_alloc_mem_info();
    return (0); 
}