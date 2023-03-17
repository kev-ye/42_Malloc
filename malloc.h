#ifndef MALLOC_H
# define MALLOC_H

/* -- Includes */

#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

/* -- Macros */

/*
* @brief Aligns pointer
* @param x size of the block
*/
#if __SIZEOF_LONG__ == 4
    #define ALIGN(x) ((((x - 1) >> 2) << 2) + __SIZEOF_LONG__)
    #define TINY_PAGE 4
    #define SMALL_PAGE 32
#elif __SIZEOF_LONG__ == 8
    #define ALIGN(x) ((((x - 1) >> 3) << 3) + __SIZEOF_LONG__)
    #define TINY_PAGE 32
    #define SMALL_PAGE 128
#else
    #error "Unsupported system architecture, it should be 32 or 64 bit"
#endif

#define BLOCK_SZIE sizeof(block_t)

#define TINY_ZONE getpagesize() * TINY_PAGE
#define SMALL_ZONE getpagesize() * SMALL_PAGE

#define TINY_BLOCK_SIZE TINY_ZONE / 128
#define SMALL_BLOCK_SIZE SMALL_ZONE / 128

#define TRUE 1
#define FALSE 0

enum {
    TINY = 1,
    SMALL,
    LARGE
};

#define DEFINE_BLOCK_SIZE(size) ((size + BLOCK_SZIE <= TINY_BLOCK_SIZE) ? TINY_ZONE : \
                                (size + BLOCK_SZIE <= SMALL_BLOCK_SIZE) ? SMALL_ZONE : size + BLOCK_SZIE)
#define DEFINE_BLOCK_ZONE(size) ((size + BLOCK_SZIE <= TINY_BLOCK_SIZE) ? TINY : \
                                (size + BLOCK_SZIE <= SMALL_BLOCK_SIZE) ? SMALL : LARGE)

/* -- Structures */

typedef struct s_block block_t;
struct s_block {
    uint8_t     zone : 2;
    uint8_t     is_free : 2;
    size_t      size;
    block_t*    next;
};

#endif
