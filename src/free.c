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
	if (DEBUG) {
		ft_putstr_fd("\n-- "S_CYAN"Free()"S_NONE" called: ", STDOUT_FILENO);
		ft_putstr_fd("["S_GREEN, STDOUT_FILENO);
		ft_putaddr_fd(ptr, STDOUT_FILENO);
		ft_putstr_fd(S_NONE"]", STDOUT_FILENO);
		ft_putstr_fd("\n", STDOUT_FILENO);
	}

	pthread_mutex_lock(&g_memory_mutex);
	_free(ptr);
	pthread_mutex_unlock(&g_memory_mutex);

	if (DEBUG)
		ft_putstr_fd("  -> "S_CYAN"Free()"S_NONE" finished\n\n", STDOUT_FILENO);
}

/*
 * Destructor function that will be called when the program exits
 * It will free all the memory zones that are not used anymore
 */
__attribute__((destructor))
void			dealloc_free_zone() {
	if (DEBUG)
		ft_putstr_fd("\n-- "S_PURPLE"dealloc_free_zone()"S_NONE" called\n", STDOUT_FILENO);

	block_t*	to_dealloc = NULL;
	block_t*	next_zone = NULL;
	block_t*	curr_zone = g_first_block;
	size_t		dealloc_count = 0;

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
			if (DEBUG) {
				ft_putstr_fd("   --> dealloc: ["S_GREEN, STDOUT_FILENO);
				ft_putaddr_fd(to_dealloc, STDOUT_FILENO);
				ft_putstr_fd(S_NONE"]", STDOUT_FILENO);
				ft_putstr_fd("["S_GREEN, STDOUT_FILENO);
				ft_putnbr_fd(to_dealloc->size, STDOUT_FILENO, 10);
				ft_putstr_fd(S_NONE" bytes]", STDOUT_FILENO);
				ft_putstr_fd("\n", STDOUT_FILENO);
			}
			if (munmap(to_dealloc, to_dealloc->size) != 0) {
				if (DEBUG)
					ft_putstr_fd(S_RED"Error: munmap failed\n"S_NONE, STDERR_FILENO);
				return;
			}
			to_dealloc = NULL;
			g_first_block = next_zone;
			++dealloc_count;
		}	
		curr_zone = next_zone;
	}

	if (DEBUG && dealloc_count == 0)
		ft_putstr_fd("  --> "S_YELLOW"No zone to deallocate\n"S_NONE, STDOUT_FILENO);

}