/* SPDX-License-Identifier: MIT */

#ifndef CONFIG_NOLIBC
# error "This file should only be compiled for no libc build"
#endif

#include "lib.h"
#include "syscall.h"

void *memset(void *s, int c, size_t n)
{
	size_t i;
	unsigned char *p = s;

	for (i = 0; i < n; i++)
		p[i] = (unsigned char) c;

	return s;
}

struct uring_heap {
	size_t		len;
	char		user_p[];
};

void *malloc(size_t len)
{
	struct uring_heap *heap;

	heap = uring_mmap(NULL, sizeof(*heap) + len, PROT_READ | PROT_WRITE,
			  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (IS_ERR(heap))
		return NULL;

	heap->len = sizeof(*heap) + len;
	return heap->user_p;
}

void free(void *p)
{
	struct uring_heap *heap;

	if (uring_unlikely(!p))
		return;

	heap = container_of(p, struct uring_heap, user_p);
	uring_munmap(heap, heap->len);
}