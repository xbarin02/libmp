#ifndef HSORT_H
#define HSORT_H

#include <stddef.h>

void mp_hsort(void *base, size_t num, size_t size,
	int (*cmp_func)(const void *, const void *),
	void (*swap_func)(void *, void *, int size));

void mp_hsort_i64_u128(void *base, size_t num);

#endif
