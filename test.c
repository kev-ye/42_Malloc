#include <stdio.h>
#include <sys/mman.h>

int main() {
    int N = 1;
    int *ptr = mmap (NULL, N * sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
    if(ptr == MAP_FAILED){
        printf("1. Mapping Failed\n");
        return 1;
    }

    int *ptr2 = mmap(ptr, N * sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
    if(ptr2 == MAP_FAILED){
        printf("2. Mapping Failed\n");
        return 1;
    }

    printf("addr 1: [%p]\n", ptr);
    printf("addr 2: [%p]\n", ptr2);
    int err = munmap(ptr, N * sizeof(int));
    if(err != 0){
        printf("UnMapping Failed\n");
        return 1;
    }

    err = munmap(ptr2, N * sizeof(int));
    if(err != 0){
        printf("UnMapping Failed\n");
        return 1;
    }

    return 0;
}