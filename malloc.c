#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc.h"

block_t*	first_block = NULL;

/* Malloc */

block_t*	last_block() {
	block_t *b = first_block;

	for (; b && b->next; b = b->next)
		;
	return b;
}

void		init_block_zone(block_t *bz, size_t zsize) {
	bz->zone = DEFINE_BLOCK_ZONE(zsize);
	bz->is_free = FALSE;
	bz->size = zsize;
	bz->next = NULL;
	bz->prev = NULL;
}

void		split_block(block_t *current_b, size_t size) {
	block_t second_b;
	
	memcpy(&second_b, current_b, BLOCK_SIZE);
	current_b->zone = second_b.zone;
	current_b->is_free = FALSE;
	current_b->size = size + BLOCK_SIZE;
	current_b->prev = second_b.prev; 
	current_b->next = (void *)current_b + size + BLOCK_SIZE;
	
	current_b->next->zone = second_b.zone;
	current_b->next->is_free = TRUE;
	current_b->next->size = second_b.size - current_b->size - BLOCK_SIZE;
	current_b->next->prev = current_b;
	current_b->next->next = second_b.next;
}

block_t*	find_free_block(size_t size) {
	for (block_t *b = first_block; b != NULL; b = b->next) {
		if (b->is_free && b->size >= size + BLOCK_SIZE) {// >= ?
			split_block(b, size);
			return b;
		}
	}
	return NULL;
}

void*		ft_malloc(size_t size) {
	block_t *b = NULL;

	if (size <= 0)
		return NULL;

	size_t align_size = ALIGN(size);
	size_t zone_size = DEFINE_BLOCK_SIZE(align_size);

	if (first_block == NULL) {
		b = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		first_block = b;
		if (first_block == MAP_FAILED)
			return NULL;
		init_block_zone(first_block, zone_size);
		split_block(b, align_size);
	}
	else {
		b = find_free_block(align_size);
		if (b == NULL) {
			block_t *last_b = last_block();
			b = mmap((void *)last_b, zone_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			if (first_block == MAP_FAILED)
				return NULL;
			init_block_zone(b, zone_size);
			b->prev = last_b;
			split_block(b, align_size);
		}
	}
	return b ? b + BLOCK_SIZE : NULL;
}

/* Free */

block_t*	merge_free_block(block_t *b) {
    if (b->next && b->next->is_free) {
        b->size = b->size + BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        if (b->next) {
            b->next->prev = b;
        }
    }
    return b;
}

// void	ft_free(void *ptr) {
// 	if (ptr == NULL)
// 		return;

// 	block_t*	b = ptr - BLOCK_SIZE;
// 	b->is_free = TRUE;
// }

int main() {
	char	*s = ft_malloc(sizeof(char) * 6);

	s[0] = 'h';
	s[1] = 'e';
	s[2] = 'l';
	s[3] = 'l';
	s[4] = 'o';
	s[5] = '\0';

	printf("[%s]\n", s);
	return 0;
}