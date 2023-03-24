#ifndef MALLOC_H
# define MALLOC_H

/* -- Includes */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h> // delete later
#include <sys/mman.h>

/* -- Macros */

/*
* @brief Aligns pointer
* @param x size of the block
*/
#if __SIZEOF_LONG__ == 4
    #define ALIGN(x) ((((x - 1) >> 2) << 2) + __SIZEOF_LONG__)
    #define TINY_PAGE 4U
    #define SMALL_PAGE 32U
#elif __SIZEOF_LONG__ == 8
    #define ALIGN(x) ((((x - 1) >> 3) << 3) + __SIZEOF_LONG__)
    #define TINY_PAGE 32U
    #define SMALL_PAGE 128U
#else
    #error "Unsupported system architecture, it should be 32 or 64 bit"
#endif

#define BLOCK_SIZE sizeof(block_t)

#define TINY_ZONE getpagesize() * TINY_PAGE
#define SMALL_ZONE getpagesize() * SMALL_PAGE

#define TINY_BLOCK_SIZE TINY_ZONE / 128U
#define SMALL_BLOCK_SIZE SMALL_ZONE / 128U

#define TRUE 1
#define FALSE 0

enum {
    TINY,
    SMALL,
    LARGE
};


#define DEFINE_BLOCK_SIZE(size) ((size + BLOCK_SIZE <= TINY_BLOCK_SIZE) ? TINY_ZONE : \
                                (size + BLOCK_SIZE <= SMALL_BLOCK_SIZE) ? SMALL_ZONE : size + BLOCK_SIZE)
#define DEFINE_BLOCK_ZONE(zsize) ((zsize <= TINY_ZONE) ? TINY : \
                                (zsize <= SMALL_ZONE) ? SMALL : LARGE)
#define GET_BLOCK(ptr) ((void *)ptr - BLOCK_SIZE)
#define GET_META(ptr) ((void *)ptr + BLOCK_SIZE)

/* -- Structures */

typedef struct s_block block_t;
struct s_block {
    uint8_t     zone : 2;
    uint8_t     is_free : 2;
    size_t      size;
    block_t*    prev;
    block_t*    next;
};

/* -- Global */

extern block_t* first_block;

/* -- Prototypes */

void	    split_block(block_t *current_b, size_t size);
block_t*	merge_free_block(block_t *b);

void*	    ft_memcpy(void *dst, const void *src, size_t n);

void*   	ft_malloc(size_t size);
void*       ft_realloc(void *ptr, size_t size);
void    	ft_free(void *ptr);

void    	show_alloc_mem(void);
void    	show_alloc_mem_info(void);

#endif
