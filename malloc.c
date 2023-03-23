#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc.h"

block_t*	first_block = NULL;

/* Debug */

void	debug_bar() {
	printf("\033[1;32m""\n--- Debug Mode ---\n""\033[0m");
	if (first_block == NULL) {
		printf("No blocks allocate!\n");
		return;
	}

	size_t total_malloc_len = 0;
	size_t total_r_allocated_len = 0;
	size_t total_v_allocated_len = 0;

	printf("Info Mode:\n");
	for (block_t *b = first_block; b != NULL; b = b->next) {
		total_malloc_len += b->size;
		if (b->is_free == FALSE) {
			total_r_allocated_len += b->size;
			total_v_allocated_len += b->size >= 32 ? b->size - BLOCK_SIZE : b->size;
		}
		printf("[%p][%s][%s]", (void *)b, b->zone == TINY ? "TINY" : b->zone == SMALL ? "SMALL" : "LARGE", b->is_free ? "F" : "A");
		printf("[r: %6zu b]", b->size);
		printf("[v: %6zu b]", b->size >= 32 ? b->size - BLOCK_SIZE : b->size);
		if (b->next)
			printf("[va: %p]\n", (void *)b + BLOCK_SIZE);
		else
			printf("\n");
		if (b->next && b->zone != b->next->zone)
			printf("---\n");
	}

	size_t bar_len, zero = 0;

	printf("\nBar Mode:\n%s: [", first_block->zone == TINY ? "TINY" : first_block->zone == SMALL ? "SMALL" : "LARGE");
	for (block_t *b = first_block; b != NULL; b = b->next) {
		if (b->is_free)
			printf("F");
		else
			printf("A");
		if (b->next)
			printf(" ");
		if (b->next && b->zone != b->next->zone)
			printf("]\n%s: [", b->next->zone == TINY ? "TINY" : b->next->zone == SMALL ? "SMALL" : "LARGE");
	}
	printf("]\n\n");

	printf("Total allocate bytes (real):    %zu bytes\n", total_r_allocated_len);
	printf("Total allocate bytes (virtual): %zu bytes\n", total_v_allocated_len);
	printf("Total malloc len:               %zu bytes\n", total_malloc_len);
}

void	show_alloc_mem() {
	if (first_block == NULL)
		return;

	const char	*zone_name[] = {"TINY", "SMALL", "LARGE"};
	size_t		total = 0;

	printf("%s : %p\n", zone_name[first_block->zone], first_block);
	for (block_t *b = first_block; b != NULL; b = b->next) {
		if (b->is_free == FALSE) {
			printf("%p - %p : %zu bytes\n", (void *)b + BLOCK_SIZE, (void *)b + b->size, b->size >= BLOCK_SIZE ? b->size- BLOCK_SIZE : b->size);
			total += b->size;
		}
		if (b->next && b->zone != b->next->zone) 
			printf("%s : %p\n", zone_name[first_block->zone], b);
	}
	printf("Total : %zu bytes\n", total);
}

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
	current_b->next->size = second_b.size - current_b->size;
	current_b->next->prev = current_b;
	current_b->next->next = second_b.next;
}

block_t*	find_free_block(size_t size) {
	for (block_t *b = first_block; b != NULL; b = b->next) {
		if (b->is_free && b->size > size + (BLOCK_SIZE * 2)) {
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

	b = find_free_block(align_size);
	if (b == NULL) {
		b = mmap(last_block(), zone_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (b == MAP_FAILED)
			return NULL;
		
		init_block_zone(b, zone_size);
		split_block(b, align_size);

		if (!first_block) {
			first_block = b;
		}
		else {
			block_t *last_b = last_block();
			last_b->next = b;
		}
	}
	return b ? (void *)b + BLOCK_SIZE : NULL;
}

/* Free */

block_t*	merge_free_block(block_t *b) {
    if (b->next && b->next->is_free) {
        b->size = b->size + b->next->size;
        b->next = b->next->next;
        if (b->next) {
            b->next->prev = b;
        }
    }
	return b;
}

block_t*	get_current_block(void *ptr) {
	for (block_t *b = first_block; b != NULL; b = b->next) {
		if ((void *)b == ptr)
			return b;
	}
	return NULL;
}

void	ft_free(void *ptr) {
	if (ptr == NULL)
		return;

	block_t*	b = (void *)ptr - BLOCK_SIZE;

	if (get_current_block((void *)b) == NULL)
		return;

	b->is_free = TRUE;
	if (b->prev && b->prev->is_free) {
		b = merge_free_block(b->prev);
	}
	if (b->next) {
		merge_free_block(b);
	}
}

__attribute__((destructor))
void	dealloc_free_zone() {
	block_t	*to_dealloc = NULL;
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
		debug_bar();
	else
		printf("\nNo more allocated memory\n");
}

// int main() {
// 	char	*s = ft_malloc(sizeof(char) * 4000);
// 	debug_bar();

// 	ft_free(s);

// 	char	*s2 = ft_malloc(sizeof(char) * TINY_ZONE);
// 	debug_bar();

// 	ft_free(s2);
// 	return 0;
// }