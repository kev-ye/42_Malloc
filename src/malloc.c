#include "malloc.h"


block_t*		g_first_block = NULL;
pthread_mutex_t	g_memory_mutex = PTHREAD_MUTEX_INITIALIZER;


static block_t*	_last_block() {
	block_t *b = g_first_block;

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

static block_t*	_find_free_block(size_t size) {
	for (block_t *b = g_first_block; b != NULL; b = b->next) {
		if (b->is_free && b->size > size + (BLOCK_SIZE * 2)) {
			split_block(b, size);
			return b;
		}
	}
	return NULL;
}

void*			_malloc(size_t size) {
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
		split_block(b, align_size);

		if (!g_first_block) {
			g_first_block = b;
		}
		else {
			block_t *last_b = _last_block();
			last_b->next = b;
		}
	}
	return b ? GET_BLOCK(b) : NULL;
}

void			*malloc(size_t size)
{
	void	*ptr = NULL;

	pthread_mutex_lock(&g_memory_mutex);
	ptr = _malloc(size);
	pthread_mutex_unlock(&g_memory_mutex);

	return (ptr);
}
