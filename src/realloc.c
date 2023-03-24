#include "malloc.h"

void*		ft_realloc(void *ptr, size_t size) {
    if (ptr == NULL)
        return (ft_malloc(size));

    block_t*    free_block = NULL;
    block_t*    old_block = GET_META(ptr);
    size_t      align_size = ALIGN(size);
    void*       new_ptr = NULL;

    if (old_block->is_free == TRUE)
        return NULL;

    if (old_block->size >= align_size + BLOCK_SIZE) { // need check it "+ BLOCK_SIZE"
        split_block(old_block, align_size);
        free_block = old_block->next;
        merge_free_block(free_block);
        return ptr;
    }

    if (old_block->next &&
        old_block->next->is_free == TRUE &&
        old_block->next->size + old_block->size >= align_size + BLOCK_SIZE &&
        old_block->next->zone == old_block->zone) { // need check it "+ BLOCK_SIZE"
        old_block->is_free = TRUE;
        merge_free_block(free_block);
        old_block->is_free = FALSE;
        split_block(old_block, align_size);
		return ptr;
    }
    else {
        if ((new_ptr = ft_malloc(size)) == NULL)
            return NULL;
        ft_memcpy(new_ptr, ptr, old_block->size - BLOCK_SIZE);
        ft_free(ptr);
        return new_ptr;
    }
    return ptr;
}