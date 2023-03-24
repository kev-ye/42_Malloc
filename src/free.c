#include "malloc.h"

/* Free */

static block_t*	_merge_free_block(block_t *b) {
    if (b->next && b->next->is_free) {
        b->size = b->size + b->next->size;
        b->next = b->next->next;
        if (b->next) {
            b->next->prev = b;
        }
    }
	return b;
}

static block_t*	_get_current_block(void *ptr) {
	for (block_t *b = first_block; b != NULL; b = b->next) {
		if ((void *)b == ptr)
			return b;
	}
	return NULL;
}

void	ft_free(void *ptr) {
	if (ptr == NULL)
		return;

	block_t*    b = (void *)ptr - BLOCK_SIZE;

	if (_get_current_block((void *)b) == NULL)
		return;

	b->is_free = TRUE;
	if (b->prev && b->prev->is_free) {
		b = _merge_free_block(b->prev);
	}
	if (b->next) {
		_merge_free_block(b);
	}
}

__attribute__((destructor))
void	dealloc_free_zone() {
	block_t *to_dealloc = NULL;
	block_t *next_zone = NULL;
	block_t *curr_zone = first_block;

	while (curr_zone) {
		for (block_t *b = curr_zone; b != NULL; b = b->next) {
			if (b->next && b->next->zone != curr_zone->zone) {
				next_zone = b->next;
				break;
			}
			else if (b->next == NULL) {
				next_zone = NULL;
			}
		}
		to_dealloc = curr_zone;
		for (block_t *b = to_dealloc; b->next && b->next != next_zone; b = b->next) {
			if (b->is_free == FALSE) {
				to_dealloc = NULL;
				break;
			}
		}

		if (to_dealloc) {
			if (munmap(to_dealloc, to_dealloc->size) != 0) {
				printf("\n\nError: munmap failed\n\n");
			}
			to_dealloc = NULL;
			first_block = next_zone;
		}
		curr_zone = next_zone;
	}
	if (first_block) 
		show_alloc_mem_info();
	else
		printf("\nNo more allocated memory\n");
}