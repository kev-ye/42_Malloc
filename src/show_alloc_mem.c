#include "malloc.h"


void	show_alloc_mem_info() {
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

void	show_alloc_mem(void) {
	if (first_block == NULL)
		return;

	const char  *zone_name[] = {"TINY", "SMALL", "LARGE"};
	size_t      total = 0;

	printf("%s : %p\n", zone_name[first_block->zone], first_block);
	for (block_t *b = first_block; b != NULL; b = b->next) {
		if (b->is_free == FALSE) {
			printf("%p - %p : %zu bytes\n", (void *)b + BLOCK_SIZE, (void *)b + b->size, b->size >= BLOCK_SIZE ? b->size- BLOCK_SIZE : b->size);
			total += b->size;
		}
		if (b->next && b->zone != b->next->zone) 
			printf("%s : %p\n", zone_name[b->next->zone], b);
	}
	printf("Total : %zu bytes\n", total);
}