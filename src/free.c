#include "malloc.h"


static block_t*	_get_current_block(void *ptr) {
	for (block_t *b = g_first_block; b != NULL; b = b->next) {
		if ((void *)b == ptr)
			return b;
	}
	return NULL;
}

void			_free(void *ptr) {
	if (ptr == NULL)
		return;

	block_t*    b = GET_META(ptr);


	// check if the pointer is valid
	if (_get_current_block(b) == NULL) {
		ft_putstr_fd(S_YELLOW"Error: invalid pointer\n"S_NONE, STDERR_FILENO);
		return;
	}

	// check if the pointer is already free
	if (b->is_free == TRUE) {
		ft_putstr_fd(S_YELLOW"Error: double free\n"S_NONE, STDERR_FILENO);
		return;
	}

	b->is_free = TRUE;
	if (b->prev && b->prev->is_free)
		b = merge_free_block(b->prev);
	if (b->next)
		merge_free_block(b);
}

void			free(void *ptr) {
	pthread_mutex_lock(&g_memory_mutex);
	_free(ptr);
	pthread_mutex_unlock(&g_memory_mutex);
}

/*
 * Destructor function that will be called when the program exits
 * It will free all the memory zones that are not used anymore
 */
__attribute__((destructor))
void			dealloc_free_zone() {
	block_t *to_dealloc = NULL;
	block_t *next_zone = NULL;
	block_t *curr_zone = g_first_block;

	while (curr_zone) {
		next_zone = get_next_zone(curr_zone);
		to_dealloc = curr_zone;
		for (block_t *b = to_dealloc; b->next && b->next != next_zone; b = b->next) {
			if (b->is_free == FALSE) {
				to_dealloc = NULL;
				break;
			}
		}

		if (to_dealloc) {
			if (munmap(to_dealloc, to_dealloc->size) != 0) {
				ft_putstr_fd(S_YELLOW"Error: munmap failed\n"S_NONE, STDERR_FILENO);
				return;
			}
			to_dealloc = NULL;
			g_first_block = next_zone;
		}
		curr_zone = next_zone;
	}
}