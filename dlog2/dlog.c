#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <libmp.h>

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
	assert( p > INT64_0 && (p & INT64_1) );

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
	int64_t x = (*(const int64_t *)p2) - (*(const int64_t *)p1);
	return (x < 0) ? -1 : (x > 0);
}

// http://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
static
int64_t int64_ceil_div(int64_t a, int64_t b)
{
	return (a + b - INT64_1) / b;
}

// https://en.wikipedia.org/wiki/Baby-step_giant-step
static
int64_t dlog2_bga_qsort(int64_t p)
{
	assert( p > INT64_0 && (p & INT64_1) );

	if( INT64_1 == p ) return INT64_0;

	int64_t m = int64_ceil_div(int64_ceil_sqrt(p), 3);

	// FIXME: timing on AMD Athlon 64 4000+
	// FIXME: 1k .. 64s // 2k .. 42s // 4k .. 35s // 8k .. 43s // 16k .. 56s // 32k .. 56s // 512k .. 56s // dlog2_lsb .. 107s // for p < 1000000 // sqrt(p)/3 .. 33s
	// FIXME: 1k .. 5415s // 2k .. 3107s // 4k .. 1826s // 8k .. 1261s // 16k ..1258s // 32k .. 1758s // 64k .. 2070s // 512k .. 2078s // dlog2_lsb .. 9551s // for p < 10000000 // sqrt(p)/3 .. 1149s
	size_t cache_size = 1<<20;
	if( 2*m*sizeof(int64_t) > cache_size )
		m = cache_size/2/sizeof(int64_t);
	int64_t n = int64_ceil_div(p, m);

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
	for(int64_t i = INT64_1; i < n; i++)
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
int64_t dlog2_bga_qsort_limit(int64_t p, int64_t exponent_limit)
{
	assert( p > INT64_0 && (p & INT64_1) );

	if( INT64_1 == p ) return INT64_0;

	int64_t m = int64_ceil_div(int64_ceil_sqrt(p), 3);

	size_t cache_size = 1<<20;
	if( 2*m*sizeof(int64_t) > cache_size )
		m = cache_size/2/sizeof(int64_t);
	int64_t n = int64_ceil_div(p, m);

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

	// i*m < exponent_limit
	int64_t i_limit = int64_ceil_div(exponent_limit, m);

	int64_t y = am;
	for(int64_t i = INT64_1; i < n && i <= i_limit; i++)
	{
		const int64_t *res = bsearch_(&y, tab, m, 2*sizeof(int64_t), cmp_int64);
		if( res )
		{
			return i*m + *(res+1);
		}

		y *= am;
		y %= p;
	}

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
	switch(argc)
	{
		case 1:
			printf("Usage:\n\t%s [f]\t.. returns dlog2(1) (mod f)\n\t%s [f] [r]\t.. returns 2^r (mod f)\n", *argv, *argv);
			break;
		case 2:
		{
			int64_t f = atol(argv[1]);
			printf("dlog2(%" PRId64 ") = %" PRId64 " (MSB)\n", f, mp_int64_dlog2_pl(f));
			printf("dlog2(%" PRId64 ") = %" PRId64 " (LSB)\n", f, mp_int64_dlog2_mn(f));
			printf("dlog2(%" PRId64 ") = %" PRId64 " (BGA)\n", f, mp_int64_dlog2_bg(f));
		}
			break;
		case 3:
		{
			int64_t f = atol(argv[1]);
			int64_t r = atol(argv[2]);
			printf("(mod %" PRId64 ") 2^(+%" PRId64 ") = %" PRId64 " (MSB)\n", f, r, mp_int64_dpow2_pl(f, r));
			printf("(mod %" PRId64 ") 2^(-%" PRId64 ") = %" PRId64 " (LSB)\n", f, r, mp_int64_dpow2_mn(f, r));
		}
			break;
		default:
			return main(1, argv);
	}

	return 0;
}
