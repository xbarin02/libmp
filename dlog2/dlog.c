#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

#define INT64_0 INT64_C(0)
#define INT64_1 INT64_C(1)
#define INT64_2 INT64_C(2)

static
int64_t dlog2_msb(int64_t p)
{
	assert( p > INT64_0 );

	int64_t m = INT64_1;
	int64_t k = INT64_0;

	while( k < p - INT64_1 )
	{
		m <<= 1;
		if( m >= p )
			m -= p;

		k++;

		if( INT64_1 == m )
			return k;
		if( INT64_0 == m )
			return 0;
	}

	return INT64_0;
}

static
int64_t dlog2_r_msb(int64_t p, int64_t K)
{
	assert( p > INT64_0 );

	int64_t m = INT64_1;
	int64_t k = INT64_0;

	while( k < K )
	{
		m <<= 1;
		if( m >= p )
			m -= p;

		k++;
	}

	return m;
}

static
int64_t dlog2_lsb(int64_t p)
{
	assert( p > INT64_0 );

	int64_t m = INT64_1;
	int64_t k = INT64_0;

	while( k < p - INT64_1 )
	{
		if( m & INT64_1 )
			m += p;
		m >>= 1;

		k++;

		if( INT64_1 == m )
			return k;
		if( INT64_0 == m )
			return 0;
	}

	return INT64_0;
}

static
int64_t dlog2_r_lsb(int64_t p, int64_t K)
{
	assert( p > INT64_0 );

	int64_t m = INT64_1;
	int64_t k = INT64_0;

	while( k < K )
	{
		if( m & INT64_1 )
			m += p;
		m >>= 1;

		k++;
	}

	return m;
}

static
int64_t int64_ceil_sqrt(int64_t n)
{
	assert( n > INT64_0 );

	int64_t x = n;
	int64_t y = INT64_0;

	while(x != y)
	{
		y = x;
		x = (x + (n + x - INT64_1)/x + INT64_1) >> 1;
	}

	return x;
}

// http://lxr.free-electrons.com/source/lib/sort.c
static
void *bsearch_(const void *key, const void *base, size_t num, size_t size,
		int (*cmp)(const void *key, const void *elt))
{
	size_t start = 0, end = num;
	int result;

	while (start < end) {
		size_t mid = start + (end - start) / 2;

		result = cmp(key, (int8_t *)base + mid * size);
		if (result < 0)
			end = mid;
		else if (result > 0)
			start = mid + 1;
		else
			return (int8_t *)base + mid * size;
	}

	return NULL;
}

static
int cmp_int64(const void *p1, const void *p2)
{
	return (*(const int64_t *)p2) - (*(const int64_t *)p1);
}

// TODO
// [x] qsort, bsearch
//     * for p < 100000 is slower
//     * for p < 1000000 is much faster
//     * for p < 10000000 test takes too long
// [ ] limit tab[m] to fit into cache size
// [x] check for overflow ... https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
// [x] is 1 found order among baby steps?

// https://en.wikipedia.org/wiki/Baby-step_giant-step
static
int64_t dlog2_bga_qsort(int64_t p)
{
	assert( p > INT64_0 && (p & INT64_1) );

	if( INT64_1 == p ) return INT64_0;

	int64_t m = int64_ceil_sqrt(p);

	int64_t tab[2*m];

	int64_t aj = INT64_1;
	for(int64_t j = INT64_0; j < m; j++)
	{
		tab[2*j+0] = aj;
		tab[2*j+1] = j;
		aj <<= 1;
		if( aj >= p )
			aj -= p;
		if( INT64_1 == aj ) return j+INT64_1;
	}

	qsort(tab, m, 2*sizeof(int64_t), cmp_int64);

	int64_t am = dlog2_r_lsb(p, m);

	if( p > INT64_1 && am > INT64_MAX / (p-INT64_1) )
	{
		printf("WARNING: 'y *= am' could overflow!\n");
		return dlog2_lsb(p);
	}

	int64_t y = am;
	for(int64_t i = INT64_1; i < m; i++)
	{
		const int64_t *res = bsearch_(&y, tab, m, 2*sizeof(int64_t), cmp_int64);
		if( res )
		{
			return i*m + *(res+1);
		}

		y *= am;
		y %= p;
	}

	assert( !"failure" );
	return 0;
}

static
int64_t dlog2_bga(int64_t p)
{
	assert( p > INT64_0 && (p & INT64_1) );

	if( INT64_1 == p ) return INT64_0;

	int64_t m = int64_ceil_sqrt(p);

	int64_t tab[m];

	int64_t aj = INT64_1;
	for(int64_t j = INT64_0; j < m; j++)
	{
		tab[j] = aj;
		aj <<= 1;
		if( aj >= p )
			aj -= p;
		if( INT64_1 == aj ) return j+INT64_1;
	}

	int64_t am = dlog2_r_lsb(p, m);

	if( p > INT64_1 && am > INT64_MAX / (p-INT64_1) )
	{
		printf("WARNING: 'y *= am' could overflow!\n");
		return dlog2_lsb(p);
	}

	int64_t y = am;
	for(int64_t i = INT64_1; i < m; i++)
	{
		for(int64_t j = INT64_0; j < m; j++)
		{
			if( y == tab[j] )
			{
				return i*m + j;
			}
		}

		y *= am;
		y %= p;
	}

	assert( !"failure" );
	return 0;
}

int main(int argc, char *argv[])
{
#if 0
	for(int64_t f = 1; f < 100000; f+=2)
	{
		//if( dlog2_lsb(f) != dlog2_msb(f) )
		//if( dlog2_lsb(f) != dlog2_bga(f) )
		if( dlog2_lsb(f) != dlog2_bga_qsort(f) )
		{
			printf("failed for %" PRId64 ": %" PRId64 " %" PRId64 "\n", f, dlog2_lsb(f), dlog2_bga_qsort(f));
			abort();
		}
	}
#endif
#if 0
	volatile int64_t a;
	for(int64_t f = 1; f < 1000000; f+=2)
		//a = dlog2_lsb(f);
		//a = dlog2_msb(f);
		//a = dlog2_bga(f);
		a = dlog2_bga_qsort(f);
	(void)a;
#endif
#if 1
	switch(argc)
	{
		case 1:
			printf("Usage:\n\t%s [f]\t.. returns dlog2(f)\n\t%s [f] [r]\t.. returns residue of r=dlog2(f)\n", *argv, *argv);
			break;
		case 2:
		{
			int64_t f = atol(argv[1]);
			printf("dlog2(%" PRId64 ") = %" PRId64 " (LSB)\n", f, dlog2_lsb(f));
			printf("dlog2(%" PRId64 ") = %" PRId64 " (MSB)\n", f, dlog2_msb(f));
			printf("dlog2(%" PRId64 ") = %" PRId64 " (BGA)\n", f, dlog2_bga_qsort(f));
		}
			break;
		case 3:
		{
			int64_t f = atol(argv[1]);
			int64_t r = atol(argv[2]);
			printf("alpha(dlog2(%" PRId64 ")=%" PRId64 ") = %" PRId64 " (LSB)\n", f, r, dlog2_r_lsb(f, r));
			printf("alpha(dlog2(%" PRId64 ")=%" PRId64 ") = %" PRId64 " (MSB)\n", f, r, dlog2_r_msb(f, r));
		}
			break;
		default:
			return main(1, argv);
	}
#endif
	return 0;
}
