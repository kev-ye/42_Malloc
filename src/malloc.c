#include "malloc.h"


block_t* first_block = NULL;

static block_t*	_last_block() {
	block_t *b = first_block;

	for (; b && b->next; b = b->next)
		;
	return b;
}

static void		_init_block_zone(block_t *bz, size_t zsize) {
	bz->zone = DEFINE_BLOCK_ZONE(zsize);
	bz->is_free = FALSE;
	bz->size = zsize;
	bz->next = NULL;
	bz->prev = NULL;
}

static void		_split_block(block_t *current_b, size_t size) {
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

static block_t*	_find_free_block(size_t size) {
	for (block_t *b = first_block; b != NULL; b = b->next) {
		if (b->is_free && b->size > size + (BLOCK_SIZE * 2)) {
			_split_block(b, size);
			return b;
		}
	}
	return NULL;
}

void*		ft_malloc(size_t size) {
	if (size <= 0)
		return NULL;

	block_t	*b = NULL;
	size_t	align_size = ALIGN(size);
	size_t	zone_size = DEFINE_BLOCK_SIZE(align_size);

	b = _find_free_block(align_size);
	if (b == NULL) {
		b = mmap(_last_block(), zone_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (b == MAP_FAILED)
			return NULL;
		
		_init_block_zone(b, zone_size);
		_split_block(b, align_size);

		if (!first_block) {
			first_block = b;
		}
		else {
			block_t *last_b = _last_block();
			last_b->next = b;
		}
	}
	return b ? (void *)b + BLOCK_SIZE : NULL;
}

int main()
{
    ft_malloc(1024);
    ft_malloc(1024 * 32);
    ft_malloc(1024 * 1024);
    ft_malloc(1024 * 1024 * 16);
    ft_malloc(1024 * 1024 * 128);
    show_alloc_mem(); 
    return (0); 
}