#include "malloc.h"

void		split_block(block_t *current_b, size_t size) {
	block_t second_b;

	ft_memcpy(&second_b, current_b, BLOCK_SIZE);
	current_b->zone = second_b.zone;
	current_b->is_free = FALSE;
	current_b->size = size + BLOCK_SIZE;
	current_b->prev = second_b.prev; 
	current_b->next = (void *)current_b + size + BLOCK_SIZE;

	current_b->next->zone = second_b.zone;
	current_b->next->is_free = TRUE;
	current_b->next->size = second_b.size - current_b->size;
	current_b->next->prev = current_b;
	current_b->next->next = second_b.next;
}

block_t*	merge_free_block(block_t *b) {
    if (b->next && b->next->is_free) {
        b->size = b->size + b->next->size;
        b->next = b->next->next;
        if (b->next)
            b->next->prev = b;
    }
	return b;
}