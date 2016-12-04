#include "libmp.h"
#include "hsort.h"

#include <stdint.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <strings.h>

static
void __attribute__ ((constructor)) init()
{
	message("libmp loaded\n");
}

// use a^(+m) rather than a^(-m) in baby-step giant-step algorithm
// #define BSGS_INVERSE

// a*b (mod p)
static
int64_t int64_dmul_int128(int64_t p, int64_t a, int64_t b)
{
	return (int64_t)( ( (int128_t)a * (int128_t)b ) % (int128_t)p );
}

// a*b (mod p)
static
int64_t int64_dmul_int64_assert(int64_t p, int64_t a, int64_t b)
{
	assert( a <= INT64_MAX / b );

	return (a * b) % p;
}

// a*b (mod p)
static
int64_t int64_dmul_int64_auto(int64_t p, int64_t a, int64_t b)
{
	if( a <= INT64_MAX / b )
		return (a * b) % p;
	else
		return int64_dmul_int128(p, a, b);
}

// a*b
static
int64_t int64_mul_int64_auto(int64_t a, int64_t b)
{
	if( a <= INT64_MAX / b )
		return a * b;
	else
		abort(); // return int64_mul_int128(a, b);
}

// a*b
static
int128_t int128_mul_int128_auto(int128_t a, int128_t b)
{
	if( a <= INT128_MAX / b )
		return a * b;
	else
		abort(); // return int128_mul_int256(a, b);
}

// a*b (mod p)
static
int128_t int128_dmul_int128_assert(int128_t p, int128_t a, int128_t b)
{
	assert( a <= INT128_MAX / b );

	return (a * b) % p;
}

// 2^(+K) (mod p), exponentiation by squaring, O(log2(K)) complexity
static
int64_t int64_dpow2_pl_log(int64_t p, int64_t K)
{
	assert( p > INT64_0 );
	assert( K >= INT64_0 );
#if 1
	int64_t b = INT64_2;
	int64_t m = INT64_1;

	while( K > INT64_0 )
	{
		if( INT64_1 & K )
		{
			m = int64_dmul_int64_assert(p, m, b);
		}

		b = int64_dmul_int64_assert(p, b, b);

		K >>= 1;
	}

	return m;
#endif
#if 0
	int64_t b = INT64_2;
	int64_t m = INT64_1;

	while( K > INT64_0 )
	{
		if( INT64_1 & K )
		{
			m = int64_dmul_int128(p, m, b);
		}

		b = int64_dmul_int128(p, b, b);

		K >>= 1;
	}

	return m;
#endif
}

int64_t mp_int64_dpow2_pl_log(int64_t p, int64_t K) { return int64_dpow2_pl_log(p, K); }

// 2^(+K) (mod p), exponentiation by squaring, O(log2(K)) complexity
static
int128_t int128_dpow2_pl_log(int128_t p, int128_t K)
{
	assert( p > INT128_0 );
	assert( K >= INT128_0 );
#if 1
	int128_t b = INT128_2;
	int128_t m = INT128_1;

	while( K > INT128_0 )
	{
		if( INT128_1 & K )
		{
			m = int128_dmul_int128_assert(p, m, b);
		}

		b = int128_dmul_int128_assert(p, b, b);

		K >>= 1;
	}

	return m;
#endif
}

int128_t mp_int128_dpow2_pl_log(int128_t p, int128_t K) { return int128_dpow2_pl_log(p, K); }

// b^(+k) (mod p)
static
int64_t int64_dpow_pl_log(int64_t b, int64_t p, int64_t k)
{
	assert( p > INT64_0 );
	assert( k >= INT64_0 );

	int64_t m = INT64_1;

	while( k > INT64_0 )
	{
		if( INT64_1 & k )
		{
			m = int64_dmul_int64_auto(p, m, b);
		}

		b = int64_dmul_int64_auto(p, b, b);

		k >>= 1;
	}

	return m;
}

int64_t mp_int64_dpow_pl_log(int64_t b, int64_t p, int64_t k) { return int64_dpow_pl_log(b, p, k); }

// b^(+k) (mod p)
static
int128_t int128_dpow_pl_log(int128_t b, int128_t p, int128_t k)
{
	assert( p > INT128_0 );
	assert( k >= INT128_0 );

	int128_t m = INT128_1;

	while( k > INT128_0 )
	{
		if( INT128_1 & k )
		{
			m = int128_dmul_int128_assert(p, m, b);
		}

		b = int128_dmul_int128_assert(p, b, b);

		k >>= 1;
	}

	return m;
}

int128_t mp_int128_dpow_pl_log(int128_t b, int128_t p, int128_t k) { return int128_dpow_pl_log(b, p, k); }

// b^(+k)
static
int64_t int64_pow_pl_log(int64_t b, int64_t k)
{
	assert( k >= INT64_0 );

	int64_t m = INT64_1;

	while( k > INT64_0 )
	{
		if( INT64_1 & k )
		{
			m = int64_mul_int64_auto(m, b);
		}

		b = int64_mul_int64_auto(b, b);

		k >>= 1;
	}

	return m;
}

int64_t mp_int64_pow_pl_log(int64_t b, int64_t k) { return int64_pow_pl_log(b, k); }

// b^(+k)
static
int128_t int128_pow_pl_log(int128_t b, int128_t k)
{
	assert( k >= INT128_0 );

	int128_t m = INT128_1;

	while( k > INT128_0 )
	{
		if( INT128_1 & k )
		{
			m = int128_mul_int128_auto(m, b);
		}

		b = int128_mul_int128_auto(b, b);

		k >>= 1;
	}

	return m;
}

int128_t mp_int128_pow_pl_log(int128_t b, int128_t k) { return int128_pow_pl_log(b, k); }

// 2^(+K) (mod p), O(K) complexity
static
int64_t int64_dpow2_pl(int64_t p, int64_t K)
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

int64_t mp_int64_dpow2_pl(int64_t p, int64_t K) { return int64_dpow2_pl(p, K); }

// 2^(+K) (mod p), O(K) complexity
static
int128_t int128_dpow2_pl(int128_t p, int128_t K)
{
	assert( p > INT128_0 );

	int128_t m = INT128_1;
	int128_t k = INT128_0;

	while( k < K )
	{
		m <<= 1;
		if( m >= p )
			m -= p;

		k++;
	}

	return m;
}

int128_t mp_int128_dpow2_pl(int128_t p, int128_t K) { return int128_dpow2_pl(p, K); }

// 2^(-K) (mod p), O(K) complexity
static
int64_t int64_dpow2_mn(int64_t p, int64_t K)
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

int64_t mp_int64_dpow2_mn(int64_t p, int64_t K) { return int64_dpow2_mn(p, K); }

// 2^(-K) (mod p), O(log2(K)) complexity
static
int64_t int64_dpow2_mn_log(int64_t p, int64_t K)
{
#if 1
	int64_t m = INT64_1;
	for(int b = 8*sizeof(int64_t)-1; b >= 0; b--)
	{
		m = int64_dmul_int64_assert(p, m, m);

		if( K & (INT64_1<<b) )
		{
			if( m & INT64_1 )
				m += p;
			m >>= 1;
		}
	}
	return m;
#endif
#if 0
	int64_t m = INT64_1;
	for(int b = 8*sizeof(int64_t)-1; b >= 0; b--)
	{
		m = int64_dmul_int128(p, m, m);

		if( K & (INT64_1<<b) )
		{
			if( m & INT64_1 )
				m += p;
			m >>= 1;
		}
	}
	return m;
#endif
}

int64_t mp_int64_dpow2_mn_log(int64_t p, int64_t K) { return int64_dpow2_mn_log(p, K); }

// 2^(-K) (mod p), O(K) complexity
static
int128_t int128_dpow2_mn(int128_t p, int128_t K)
{
	assert( p > INT128_0 );

	int128_t m = INT128_1;
	int128_t k = INT128_0;

	while( k < K )
	{
		if( m & INT128_1 )
			m += p;
		m >>= 1;

		k++;
	}

	return m;
}

int128_t mp_int128_dpow2_mn(int128_t p, int128_t K) { return int128_dpow2_mn(p, K); }

// 2^(-K) (mod p), O(log2(K)) complexity
static
int128_t int128_dpow2_mn_log(int128_t p, int128_t K)
{
#if 1
	int128_t m = INT128_1;
	for(int b = 8*sizeof(int128_t)-1; b >= 0; b--)
	{
		m = int128_dmul_int128_assert(p, m, m);

		if( K & (INT128_1<<b) )
		{
			if( m & INT128_1 )
				m += p;
			m >>= 1;
		}
	}
	return m;
#endif
#if 0
	int128_t m = INT128_1;
	for(int b = 8*sizeof(int128_t)-1; b >= 0; b--)
	{
		m = int128_dmul_int128(p, m, m);

		if( K & (INT128_1<<b) )
		{
			if( m & INT128_1 )
				m += p;
			m >>= 1;
		}
	}
	return m;
#endif
}

int128_t mp_int128_dpow2_mn_log(int128_t p, int128_t K) { return int128_dpow2_mn_log(p, K); }

// x : 2^x = 1 (mod p) [LSB, faster]
static
int64_t int64_dlog2_mn(int64_t p)
{
	assert( p > INT64_0 );
	assert( p & INT64_1 );

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
			return INT64_0;
	}

	return INT64_0;
}

int64_t mp_int64_dlog2_mn(int64_t p) { return int64_dlog2_mn(p); }

// x : 2^x = 1 (mod p) [LSB, faster]
static
int128_t int128_dlog2_mn(int128_t p)
{
	assert( p > INT128_0 );
	assert( p & INT128_1 );

	int128_t m = INT128_1;
	int128_t k = INT128_0;

	while( k < p - INT128_1 )
	{
		if( m & INT128_1 )
			m += p;
		m >>= 1;

		k++;

		if( INT128_1 == m )
			return k;
		if( INT128_0 == m )
			return INT128_0;
	}

	return INT128_0;
}

int128_t mp_int128_dlog2_mn(int128_t p) { return int128_dlog2_mn(p); }

// x : 2^x = 1 (mod p) [MSB, slower]
static
int64_t int64_dlog2_pl(int64_t p)
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
			return INT64_0;
	}

	return INT64_0;
}

int64_t mp_int64_dlog2_pl(int64_t p) { return int64_dlog2_pl(p); }

// x : 2^x = 1 (mod p) [MSB, slower]
static
int128_t int128_dlog2_pl(int128_t p)
{
	assert( p > INT128_0 );

	int128_t m = INT128_1;
	int128_t k = INT128_0;

	while( k < p - INT128_1 )
	{
		m <<= 1;
		if( m >= p )
			m -= p;

		k++;

		if( INT128_1 == m )
			return k;
		if( INT128_0 == m )
			return INT128_0;
	}

	return INT128_0;
}

int128_t mp_int128_dlog2_pl(int128_t p) { return int128_dlog2_pl(p); }

static
int64_t int64_min(int64_t a, int64_t b)
{
	return a < b ? a : b;
}

static
int128_t int128_min(int128_t a, int128_t b)
{
	return a < b ? a : b;
}

// x in [0; L) : 2^x = 1 (mod p) [LSB, faster]
static
int64_t int64_dlog2_mn_lim(int64_t p, int64_t L)
{
	assert( p > INT64_0 );
	assert( p & INT64_1 );

	int64_t K = int64_min(p, L) - INT64_1;

	int64_t m = INT64_1;
	int64_t k = INT64_0;

	while( k < K )
	{
		if( m & INT64_1 )
			m += p;
		m >>= 1;

		k++;

		if( INT64_1 == m )
			return k;
	}

	return INT64_0;
}

int64_t mp_int64_dlog2_mn_lim(int64_t p, int64_t L) { return int64_dlog2_mn_lim(p, L); }

// x in [0; L) : 2^x = 1 (mod p) [LSB, faster]
static
int128_t int128_dlog2_mn_lim(int128_t p, int128_t L)
{
	assert( p > INT128_0 );
	assert( p & INT128_1 );

	int128_t K = int128_min(p, L) - INT128_1;

	int128_t m = INT128_1;
	int128_t k = INT128_0;

	while( k < K )
	{
		if( m & INT128_1 )
			m += p;
		m >>= 1;

		k++;

		if( INT128_1 == m )
			return k;
	}

	return INT128_0;
}

int128_t mp_int128_dlog2_mn_lim(int128_t p, int128_t L) { return int128_dlog2_mn_lim(p, L); }

// x in [0; L) : 2^x = 1 (mod p) [MSB, slower]
static
int64_t int64_dlog2_pl_lim(int64_t p, int64_t L)
{
	assert( p > INT64_0 );
	assert( p & INT64_1 );

	int64_t K = int64_min(p, L) - INT64_1;

	int64_t m = INT64_1;
	int64_t k = INT64_0;

	while( k < K )
	{
		m <<= 1;
		if( m >= p )
			m -= p;

		k++;

		if( INT64_1 == m )
			return k;
	}

	return INT64_0;
}

int64_t mp_int64_dlog2_pl_lim(int64_t p, int64_t L) { return int64_dlog2_pl_lim(p, L); }

// x in [0; L) : 2^x = 1 (mod p) [MSB, slower]
static
int128_t int128_dlog2_pl_lim(int128_t p, int128_t L)
{
	assert( p > INT128_0 );
	assert( p & INT128_1 );

	int128_t K = int128_min(p, L) - INT128_1;

	int128_t m = INT128_1;
	int128_t k = INT128_0;

	while( k < K )
	{
		m <<= 1;
		if( m >= p )
			m -= p;

		k++;

		if( INT128_1 == m )
			return k;
	}

	return INT128_0;
}

int128_t mp_int128_dlog2_pl_lim(int128_t p, int128_t L) { return int128_dlog2_pl_lim(p, L); }

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

int64_t mp_int64_ceil_sqrt(int64_t n) { return int64_ceil_sqrt(n); }

static
int128_t int128_ceil_sqrt(int128_t n)
{
	assert( n > INT128_0 );

	int128_t x = n;
	int128_t y = INT128_0;

	while(x != y)
	{
		y = x;
		x = (x + (n + x - INT128_1)/x + INT128_1) >> 1;
	}

	return x;
}

int128_t mp_int128_ceil_sqrt(int128_t n) { return int128_ceil_sqrt(n); }

static
int64_t int64_floor_sqrt(int64_t n)
{
	assert( n > INT64_0 );

	int64_t x = n;
	int64_t y = INT64_0;

	do {
		y = x;
		x = (x + n/x) >> 1;
	} while (x < y);

	return y;
}

int64_t mp_int64_floor_sqrt(int64_t n) { return int64_floor_sqrt(n); }

static
int128_t int128_floor_sqrt(int128_t n)
{
	assert( n > INT128_0 );

	int128_t x = n;
	int128_t y = INT128_0;

	do {
		y = x;
		x = (x + n/x) >> 1;
	} while (x < y);

	return y;
}

int128_t mp_int128_floor_sqrt(int128_t n) { return int128_floor_sqrt(n); }

// http://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
static
int64_t int64_ceil_div(int64_t a, int64_t b)
{
	return (a + b - INT64_1) / b;
}

int64_t mp_int64_ceil_div(int64_t a, int64_t b) { return int64_ceil_div(a, b); }

static
int128_t int128_ceil_div(int128_t a, int128_t b)
{
	return (a + b - INT128_1) / b;
}

int128_t mp_int128_ceil_div(int128_t a, int128_t b) { return int128_ceil_div(a, b); }

static
int int64_cmp(const void *p1, const void *p2)
{
#if 0
	// BUG: overflows when casted to the int
	return (int)( (*(const int64_t *)p2) - (*(const int64_t *)p1) );
#endif
#if 0
	// BUG: overflows when the difference not fit into int64_t
	int64_t x = (*(const int64_t *)p2) - (*(const int64_t *)p1);
	return (x < 0) ? -1 : (x > 0);
#endif
#if 1
	return (*(const int64_t *)p2 < *(const int64_t *)p1) ? -1 : (*(const int64_t *)p2 > *(const int64_t *)p1);
#endif
}

static
int int128_cmp(const void *p1, const void *p2)
{
#if 0
	// BUG: overflows when casted to the int
	return (int)( (*(const int128_t *)p2) - (*(const int128_t *)p1) );
#endif
#if 0
	// BUG: overflows when the difference not fit into int64_t
	int128_t x = (*(const int128_t *)p2) - (*(const int128_t *)p1);
	return (x < 0) ? -1 : (x > 0);
#endif
#if 1
	return (*(const int128_t *)p2 < *(const int128_t *)p1) ? -1 : (*(const int128_t *)p2 > *(const int128_t *)p1);
#endif
}

// http://lxr.free-electrons.com/source/lib/bsearch.c
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

// x in [0; L) : 2^x = 1 (mod p)
static
int64_t int64_dlog2_bg_lim_mul128(int64_t p, int64_t L)
{
	assert( p > INT64_0 );
	assert( p & INT64_1 );

	if( INT64_1 == p )
		return INT64_0;

	int64_t m = int64_ceil_div(int64_ceil_sqrt(p), INT64_C(3));

	size_t cache_size = 1<<20;
	if( 2*(size_t)m*sizeof(int64_t) > cache_size )
		m = (int64_t)( cache_size/2/sizeof(int64_t) );
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

		if( INT64_1 == aj )
			return j + INT64_1;
	}

	qsort(tab, (size_t)m, 2*sizeof(int64_t), int64_cmp);

	int64_t am = int64_dpow2_mn(p, m);

	// i*m < L
	int64_t i_lim = int64_ceil_div(L, m);

	int64_t y = am;
	for(int64_t i = INT64_1; i < n && i <= i_lim; i++)
	{
		const int64_t *res = bsearch_(&y, tab, (size_t)m, 2*sizeof(int64_t), int64_cmp);
		if( res )
		{
			int64_t x = i*m + *(res+1);

			if( x < L )
				return x;
			else
				return 0;
		}

		y = int64_dmul_int128(p, y, am);
	}

	return 0;
}

static
int64_t int64_dlog2_bg_mul128(int64_t p)
{
	assert( p > INT64_0 );
	assert( p & INT64_1 );

	if( INT64_1 == p )
		return INT64_0;

	int64_t m = int64_ceil_div(int64_ceil_sqrt(p), INT64_C(3));

	size_t cache_size = 1<<20;
	if( 2*(size_t)m*sizeof(int64_t) > cache_size )
		m = (int64_t)( cache_size/2/sizeof(int64_t) );
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

		if( INT64_1 == aj )
			return j + INT64_1;
	}

	qsort(tab, (size_t)m, 2*sizeof(int64_t), int64_cmp);

	int64_t am = int64_dpow2_mn(p, m);

	int64_t y = am;
	for(int64_t i = INT64_1; i < n; i++)
	{
		const int64_t *res = bsearch_(&y, tab, (size_t)m, 2*sizeof(int64_t), int64_cmp);
		if( res )
		{
			return i*m + *(res+1);
		}

		y = int64_dmul_int128(p, y, am);
	}

	return 0;
}

// https://en.wikipedia.org/wiki/Baby-step_giant-step
// timing on AMD Athlon 64 4000+
// 1k .. 64s // 2k .. 42s // 4k .. 35s // 8k .. 43s // 16k .. 56s // 32k .. 56s // 512k .. 56s // dlog2_lsb .. 107s // for p < 1000000 // sqrt(p)/3 .. 33s
// 1k .. 5415s // 2k .. 3107s // 4k .. 1826s // 8k .. 1261s // 16k ..1258s // 32k .. 1758s // 64k .. 2070s // 512k .. 2078s // dlog2_lsb .. 9551s // for p < 10000000 // sqrt(p)/3 .. 1149s
// x in [0; L) : 2^x = 1 (mod p)
static
int64_t int64_dlog2_bg_lim(int64_t p, int64_t L)
{
	assert( p > INT64_0 );
	assert( p & INT64_1 );

	if( INT64_1 == p )
		return INT64_0;

	int64_t m = int64_ceil_div(int64_ceil_sqrt(p), INT64_C(3));

	size_t cache_size = 1<<20;
	if( 2*(size_t)m*sizeof(int64_t) > cache_size )
		m = (int64_t)( cache_size/2/sizeof(int64_t) );
	int64_t n = int64_ceil_div(p, m);

	int64_t am = int64_dpow2_mn(p, m);

	if( p > INT64_1 && am > INT64_MAX / (p - INT64_1) )
	{
		static int warned = 0;
		if( !warned )
		{
			message(WARN "'y *= am' could overflow 64 bits! Falling to 128-bit multiplication... (this message will appear only once)\n");
			warned = 1;
		}
		return (int64_t)int64_dlog2_bg_lim_mul128(p, L);
	}

	int64_t tab[2*m];

	int64_t aj = INT64_1;
	for(int64_t j = INT64_0; j < m; j++)
	{
		tab[2*j+0] = aj;
		tab[2*j+1] = j;

		aj <<= 1;
		if( aj >= p )
			aj -= p;

		if( INT64_1 == aj )
			return j + INT64_1;
	}

	qsort(tab, (size_t)m, 2*sizeof(int64_t), int64_cmp);

	// i*m < L
	int64_t i_lim = int64_ceil_div(L, m);

	int64_t y = am;
	for(int64_t i = INT64_1; i < n && i <= i_lim; i++)
	{
		const int64_t *res = bsearch_(&y, tab, (size_t)m, 2*sizeof(int64_t), int64_cmp);
		if( res )
		{
			int64_t x = i*m + *(res+1);

			if( x < L )
				return x;
			else
				return 0;
		}

		y *= am;
		y %= p;
	}

	return 0;
}

int64_t mp_int64_dlog2_bg_lim(int64_t p, int64_t L) { return int64_dlog2_bg_lim(p, L); }

// #define BSGS_HASH
#define BSGS_HASH_BITS 4
#define BSGS_HASH_ENTRIES (INT64_1<<BSGS_HASH_BITS)
#define BSGS_HASH_MASK (BSGS_HASH_ENTRIES-INT64_1)

static
int64_t int64_dlog2_bg(int64_t p)
{
	assert( p > INT64_0 );
	assert( p & INT64_1 );

	if( INT64_1 == p )
		return INT64_0;

	int64_t m = int64_ceil_div(int64_ceil_sqrt(p), INT64_C(3));

	size_t cache_size = 1<<20;
	if( 2*(size_t)m*sizeof(int64_t) > cache_size )
	{
		static int warned = 0;
		if( !warned )
		{
			message(WARN "table will be truncated to fit into cache... (this message will appear only once)\n");
			warned = 1;
		}
		m = (int64_t)( cache_size/2/sizeof(int64_t) );
	}
	int64_t n = int64_ceil_div(p, m);

#ifndef BSGS_INVERSE
	int64_t am = int64_dpow2_mn(p, m);
#else
	int64_t am = int64_dpow2_pl(p, m);
#endif

	if( p > INT64_1 && am > INT64_MAX / (p - INT64_1) )
	{
		static int warned = 0;
		if( !warned )
		{
			message(WARN "'y *= am' could overflow 64 bits! Falling to 128-bit multiplication... (this message will appear only once)\n");
			warned = 1;
		}
		return (int64_t)int64_dlog2_bg_mul128(p);
	}

#ifndef BSGS_HASH
	int64_t tab[2*m];
#else
	int64_t hash[BSGS_HASH_ENTRIES * 2*m];
	bzero(hash, sizeof(int64_t) * (size_t)BSGS_HASH_ENTRIES * 2*(size_t)m);
#endif

	for(int64_t i = INT64_0, x = INT64_1; i < m; i++)
	{
#ifndef BSGS_HASH
		tab[2*i+0] = x;
		tab[2*i+1] = i;
#else
		{
			int64_t *entry = hash +  (BSGS_HASH_MASK&x)    * 2*m;
			int64_t *end   = hash + ((BSGS_HASH_MASK&x)+1) * 2*m;
			while( entry < end && *(entry+0) )
				entry += 2;
			assert( entry != end );
			*(entry+0) = x;
			*(entry+1) = i;
		}
#endif

#ifndef BSGS_INVERSE
		x <<= 1;
		if( x >= p )
			x -= p;
#else
		if( x & INT64_1 )
			x += p;
		x >>= 1;
#endif

		if( INT64_1 == x )
			return i + INT64_1;
	}

#ifndef BSGS_HASH
	qsort(tab, (size_t)m, 2*sizeof(int64_t), int64_cmp);
// 	mp_hsort(tab, (size_t)m, 2*sizeof(int64_t), int64_cmp, 0);
// 	mp_hsort_i64_u128(tab, (size_t)m);
#endif

	for(int64_t i = INT64_1, x = am; i < n; i++)
	{
		if( INT64_1 == x )
			return i*m;

#ifndef BSGS_HASH
		const int64_t *res = bsearch_(&x, tab, (size_t)m, 2*sizeof(int64_t), int64_cmp);
#else
		const int64_t *res = NULL;
		{
			int64_t *entry = hash +  (BSGS_HASH_MASK&x)    * 2*m;
			int64_t *end   = hash + ((BSGS_HASH_MASK&x)+1) * 2*m;
			while( entry < end && *(entry+0) )
			{
				if( x == *(entry+0) )
				{
					res = entry;
					break;
				}
				entry += 2;
			}
		}
#endif
		if( res )
		{
			return i*m + *(res+1);
		}

		x *= am;
		x %= p;
	}

	return 0;
}

int64_t mp_int64_dlog2_bg(int64_t p) { return int64_dlog2_bg(p); }

// floor(log2(n))
// e.g. 1=>0, 2=>1, 15=>3, 16=>4
// TODO: implement some faster method
// https://graphics.stanford.edu/~seander/bithacks.html
static
int64_t int64_floor_log2(int64_t n)
{
	assert( n > 0 );
#if 1
	int64_t r = 0;

	while( n >>= 1 )
	{
		r++;
	}

	return r;
#endif
}

int64_t mp_int64_floor_log2(int64_t n) { return int64_floor_log2(n); }

// floor(log2(n))
// e.g. 1=>0, 2=>1, 15=>3, 16=>4
// TODO: implement some faster method
// https://graphics.stanford.edu/~seander/bithacks.html
static
int128_t int128_floor_log2(int128_t n)
{
	assert( n > 0 );
#if 1
	int128_t r = 0;

	while( n >>= 1 )
	{
		r++;
	}

	return r;
#endif
}

int128_t mp_int128_floor_log2(int128_t n) { return int128_floor_log2(n); }

static
void int64_factors_exponents(int64_t n, int64_t *factors, int64_t *exponents)
{
	assert( n > 0 );
	assert( factors );
	assert( exponents );
#if 0
	for(int64_t f = 2; n > 1; f++)
	{
		// new factor?
		if( 0 == n % f )
		{
			*factors = f;
			*exponents = 0;

			do {
				n /= f;
				(*exponents)++;
			} while( 0 == n % f );

			// increment pointers
			factors++;
			exponents++;
		}

		// try next factor
	}
#endif
#if 0
	// 2
	if( 0 == n % 2 )
	{
		*factors = 2;
		*exponents = 0;

		do {
			n /= 2;
			(*exponents)++;
		} while( 0 == n % 2 );

		// increment pointers
		factors++;
		exponents++;
	}
	// odd factors
	for(int64_t f = 3; n > 1; f += 2)
	{
		// new factor?
		if( 0 == n % f )
		{
			*factors = f;
			*exponents = 0;

			do {
				n /= f;
				(*exponents)++;
			} while( 0 == n % f );

			// increment pointers
			factors++;
			exponents++;
		}

		// try next factor
	}
#endif
#if 1
	// 2
	if( n > 1 && 0 == n % 2 )
	{
		*factors = 2;
		*exponents = 0;

		do {
			n /= 2;
			(*exponents)++;
		} while( 0 == n % 2 );

		// increment pointers
		factors++;
		exponents++;
	}
	// 3
	if( n > 1 && 0 == n % 3 )
	{
		*factors = 3;
		*exponents = 0;

		do {
			n /= 3;
			(*exponents)++;
		} while( 0 == n % 3 );

		// increment pointers
		factors++;
		exponents++;
	}
	// 5
	if( n > 1 && 0 == n % 5 )
	{
		*factors = 5;
		*exponents = 0;

		do {
			n /= 5;
			(*exponents)++;
		} while( 0 == n % 5 );

		// increment pointers
		factors++;
		exponents++;
	}
	// 6i+{1,5}
	for(int64_t i = 1; n > 1; i++)
	{
		if( 0 == n % (6*i+1) )
		{
			*factors = (6*i+1);
			*exponents = 0;

			do {
				n /= (6*i+1);
				(*exponents)++;
			} while( 0 == n % (6*i+1) );

			// increment pointers
			factors++;
			exponents++;
		}
		if( 0 == n % (6*i+5) )
		{
			*factors = (6*i+5);
			*exponents = 0;

			do {
				n /= (6*i+5);
				(*exponents)++;
			} while( 0 == n % (6*i+5) );

			// increment pointers
			factors++;
			exponents++;
		}
	}
#endif
	// terminate the list
	*factors = 0;
	*exponents = 0;
}

void mp_int64_factors_exponents(int64_t n, int64_t *factors, int64_t *exponents) { int64_factors_exponents(n, factors, exponents); }

static
void int128_factors_exponents(int128_t n, int128_t *factors, int128_t *exponents)
{
	assert( n > 0 );
	assert( factors );
	assert( exponents );
#if 0
	for(int128_t f = 2; n > 1; f++)
	{
		// new factor?
		if( 0 == n % f )
		{
			*factors = f;
			*exponents = 0;

			do {
				n /= f;
				(*exponents)++;
			} while( 0 == n % f );

			// increment pointers
			factors++;
			exponents++;
		}

		// try next factor
	}
#endif
#if 0
	// 2
	if( 0 == n % 2 )
	{
		*factors = 2;
		*exponents = 0;

		do {
			n /= 2;
			(*exponents)++;
		} while( 0 == n % 2 );

		// increment pointers
		factors++;
		exponents++;
	}
	// odd factors
	for(int128_t f = 3; n > 1; f += 2)
	{
		// new factor?
		if( 0 == n % f )
		{
			*factors = f;
			*exponents = 0;

			do {
				n /= f;
				(*exponents)++;
			} while( 0 == n % f );

			// increment pointers
			factors++;
			exponents++;
		}

		// try next factor
	}
#endif
#if 1
	// 2
	if( n > 1 && 0 == n % 2 )
	{
		*factors = 2;
		*exponents = 0;

		do {
			n /= 2;
			(*exponents)++;
		} while( 0 == n % 2 );

		// increment pointers
		factors++;
		exponents++;
	}
	// 3
	if( n > 1 && 0 == n % 3 )
	{
		*factors = 3;
		*exponents = 0;

		do {
			n /= 3;
			(*exponents)++;
		} while( 0 == n % 3 );

		// increment pointers
		factors++;
		exponents++;
	}
	// 5
	if( n > 1 && 0 == n % 5 )
	{
		*factors = 5;
		*exponents = 0;

		do {
			n /= 5;
			(*exponents)++;
		} while( 0 == n % 5 );

		// increment pointers
		factors++;
		exponents++;
	}
	// 6i+{1,5}
	for(int128_t i = 1; n > 1; i++)
	{
		if( 0 == n % (6*i+1) )
		{
			*factors = (6*i+1);
			*exponents = 0;

			do {
				n /= (6*i+1);
				(*exponents)++;
			} while( 0 == n % (6*i+1) );

			// increment pointers
			factors++;
			exponents++;
		}
		if( 0 == n % (6*i+5) )
		{
			*factors = (6*i+5);
			*exponents = 0;

			do {
				n /= (6*i+5);
				(*exponents)++;
			} while( 0 == n % (6*i+5) );

			// increment pointers
			factors++;
			exponents++;
		}
	}
#endif
	// terminate the list
	*factors = 0;
	*exponents = 0;
}

void mp_int128_factors_exponents(int128_t n, int128_t *factors, int128_t *exponents) { int128_factors_exponents(n, factors, exponents); }

// 4.79 Algorithm Determining the order of a group element
// from Handbook of Applied Cryptography
static
int64_t int64_element2_order(int64_t p)
{
	// NOTE: assert( p : PRIME );

	// group order
	int64_t n = p - 1;
	// maximum number of factors
	int64_t max_factors = int64_floor_log2(n);

	// +1 due to terminating zero
	int64_t factors[max_factors+1];
	int64_t exponents[max_factors+1];

	// n = p1^e1 * p2^e2 * ... * pk^ek
	int64_factors_exponents(n, factors, exponents);

	int64_t t = n;

	// for each factor
	for(int64_t *f = factors, *e = exponents; *f; f++, e++)
	{
		int64_t pe = int64_pow_pl_log(*f, *e);

		t = t/pe;

		// NOTE: can the BSGS algorithm be used? base .. a1 = 2^t (mod p), step .. a1 = a1^pi (mod p)
		// NOTE: probably not, because the loop iterates 0, 1, 2, or 3 times (too small number of iterations)

		int64_t a1 = int64_dpow2_pl_log(p, t);

		while( a1 != 1 )
		{
			a1 = int64_dpow_pl_log(a1, p, *f);
			t = t * *f;
		}
	}

	return t;
}

int64_t mp_int64_element2_order(int64_t p) { return int64_element2_order(p); }

// 4.79 Algorithm Determining the order of a group element
// from Handbook of Applied Cryptography
static
int128_t int128_element2_order(int128_t p)
{
	// NOTE: assert( p : PRIME );

	// group order
	int128_t n = p - 1;
	// maximum number of factors
	int128_t max_factors = int128_floor_log2(n);

	// +1 due to terminating zero
	int128_t factors[max_factors+1];
	int128_t exponents[max_factors+1];

	// n = p1^e1 * p2^e2 * ... * pk^ek
	int128_factors_exponents(n, factors, exponents);

	int128_t t = n;

	// for each factor
	for(int128_t *f = factors, *e = exponents; *f; f++, e++)
	{
		int128_t pe = int128_pow_pl_log(*f, *e);

		t = t/pe;

		int128_t a1 = int128_dpow2_pl_log(p, t);

		while( a1 != 1 )
		{
			a1 = int128_dpow_pl_log(a1, p, *f);
			t = t * *f;
		}
	}

	return t;
}

int128_t mp_int128_element2_order(int128_t p) { return int128_element2_order(p); }

static
int64_t int64_gcd(int64_t a, int64_t b)
{
	while( b != 0 )
	{
		int64_t t = b;
		b = a % b;
		a = t;
	}

	return a;
}

int64_t mp_int64_gcd(int64_t a, int64_t b) { return int64_gcd(a, b); }

static
int128_t int128_gcd(int128_t a, int128_t b)
{
	while( b != 0 )
	{
		int128_t t = b;
		b = a % b;
		a = t;
	}

	return a;
}

int128_t mp_int128_gcd(int128_t a, int128_t b) { return int128_gcd(a, b); }

// x : a*x = 1 (mod n)
static
int64_t int64_inverse(int64_t a, int64_t n)
{
	assert( a > INT64_0 );
	assert( n > INT64_0 );
#if 1
	int64_t t = INT64_0, s = INT64_1;
	int64_t r = n;

	while( a != INT64_0 )
	{
		int64_t q = r / a;

		int64_t s1 = s;
		s = t - q * s;
		t = s1;

		int64_t a1 = a;
		a = r % a; // a = r - q * a;
		r = a1;
	}

	// not invertible
	if(r > INT64_1)
		return INT64_0;

	if(t < INT64_0)
		t += n;

	return t;
#endif
#if 0
	for(int64_t i = INT64_1; i < n; i++)
		if( INT64_1 == (i*a) % n )
			return i;

	return INT64_0;
#endif
}

int64_t mp_int64_inverse(int64_t a, int64_t n) { return int64_inverse(a, n); }

// x : a*x = 1 (mod n)
static
int128_t int128_inverse(int128_t a, int128_t n)
{
	assert( a > INT128_0 );
	assert( n > INT128_0 );
#if 1
	int128_t t = INT128_0, s = INT128_1;
	int128_t r = n;

	while( a != INT128_0 )
	{
		int128_t q = r / a;

		int128_t s1 = s;
		s = t - q * s;
		t = s1;

		int128_t a1 = a;
		a = r % a; // a = r - q * a;
		r = a1;
	}

	// not invertible
	if(r > INT128_1)
		return INT128_0;

	if(t < INT128_0)
		t += n;

	return t;
#endif
#if 0
	for(int128_t i = INT128_1; i < n; i++)
		if( INT128_1 == (i*a) % n )
			return i;

	return INT128_0;
#endif
}

int128_t mp_int128_inverse(int128_t a, int128_t n) { return int128_inverse(a, n); }

// modular division
// x : Bx = A (mod n)
static
int64_t int64_divide(int64_t A, int64_t B, int64_t n)
{
	assert( n > INT64_0 );

	if( INT64_0 == B ) { message(WARN "Division by zero!\n"); return INT64_0; }

	if( INT64_0 == A % B ) return A / B; // e.g. 38x = 380  (mod 1018)

	// GCD
	int64_t g = mp_int64_gcd(B, n);

	// http://math.stackexchange.com/q/395207/204448
	if( g != INT64_1 )
	{
		if( g == B ) { message(WARN "No solution!\n"); return INT64_0; } // e.g. 2x = 1 (mod 4)

		// A/g
		int64_t Ag = int64_divide(A, g, n);

		return int64_divide(Ag, B/g, n/g);
	}

	// inverse
	int64_t b = mp_int64_inverse(B, n);

	return A * b % n;
}

int64_t mp_int64_divide(int64_t A, int64_t B, int64_t n) { return int64_divide(A, B, n); }

// modular division
// x : Bx = A (mod n)
static
int128_t int128_divide(int128_t A, int128_t B, int128_t n)
{
	assert( n > INT128_0 );

	if( INT128_0 == B ) { message(WARN "Division by zero!\n"); return INT128_0; }

	if( INT128_0 == A % B ) return A / B; // e.g. 38x = 380  (mod 1018)

	// GCD
	int128_t g = mp_int128_gcd(B, n);

	// http://math.stackexchange.com/q/395207/204448
	if( g != INT128_1 )
	{
		if( g == B ) { message(WARN "No solution!\n"); return INT128_0; } // e.g. 2x = 1 (mod 4)

		// A/g
		int128_t Ag = int128_divide(A, g, n);

		return int128_divide(Ag, B/g, n/g);
	}

	// inverse
	int128_t b = mp_int128_inverse(B, n);

	return A * b % n;
}

int128_t mp_int128_divide(int128_t A, int128_t B, int128_t n) { return int128_divide(A, B, n); }

// x in [0; L) : 2^x = 1 (mod p)
static
int128_t int128_dlog2_bg_lim(int128_t p, int128_t L)
{
	assert( p > INT128_0 );
	assert( p & INT128_1 );

	if( INT128_1 == p )
		return INT128_0;

	int128_t m = int128_ceil_div(int128_ceil_sqrt(p), INT128_C(3));

	size_t cache_size = 1<<20;
	if( 2*m*sizeof(int128_t) > cache_size )
		m = cache_size/2/sizeof(int128_t);
	int128_t n = int128_ceil_div(p, m);

	int128_t am = int128_dpow2_mn(p, m);

	if( p > INT128_1 && am > INT128_MAX / (p - INT128_1) )
	{
		message(WARN "'y *= am' could overflow 128 bits! Falling to a naive algorithm...\n");
		return int128_dlog2_mn_lim(p, L);
	}

	int128_t tab[2*m];

	int128_t aj = INT128_1;
	for(int128_t j = INT128_0; j < m; j++)
	{
		tab[2*j+0] = aj;
		tab[2*j+1] = j;

		aj <<= 1;
		if( aj >= p )
			aj -= p;

		if( INT128_1 == aj )
			return j + INT128_1;
	}

	qsort(tab, (size_t)m, 2*sizeof(int128_t), int128_cmp);

	// i*m < L
	int128_t i_lim = int128_ceil_div(L, m);

	int128_t y = am;
	for(int128_t i = INT128_1; i < n && i <= i_lim; i++)
	{
		const int128_t *res = bsearch_(&y, tab, (size_t)m, 2*sizeof(int128_t), int128_cmp);
		if( res )
		{
			int128_t x = i*m + *(res+1);

			if( x < L )
				return x;
			else
				return 0;
		}

		y *= am;
		y %= p;
	}

	return 0;
}

int128_t mp_int128_dlog2_bg_lim(int128_t p, int128_t L) { return int128_dlog2_bg_lim(p, L); }

static
int128_t int128_dlog2_bg(int128_t p)
{
	assert( p > INT128_0 );
	assert( p & INT128_1 );

	if( INT128_1 == p )
		return INT128_0;

	int128_t m = int128_ceil_div(int128_ceil_sqrt(p), INT128_C(3));

	size_t cache_size = 1<<20;
	if( 2*m*sizeof(int128_t) > cache_size )
		m = cache_size/2/sizeof(int128_t);
	int128_t n = int128_ceil_div(p, m);

	int128_t am = int128_dpow2_mn(p, m);

	if( p > INT128_1 && am > INT128_MAX / (p - INT128_1) )
	{
		message(WARN "'y *= am' could overflow 128 bits! Falling to a naive algorithm...\n");
		return int128_dlog2_mn(p);
	}

	int128_t tab[2*m];

	int128_t aj = INT128_1;
	for(int128_t j = INT128_0; j < m; j++)
	{
		tab[2*j+0] = aj;
		tab[2*j+1] = j;

		aj <<= 1;
		if( aj >= p )
			aj -= p;

		if( INT128_1 == aj )
			return j + INT128_1;
	}

	qsort(tab, (size_t)m, 2*sizeof(int128_t), int128_cmp);

	int128_t y = am;
	for(int128_t i = INT128_1; i < n; i++)
	{
		const int128_t *res = bsearch_(&y, tab, (size_t)m, 2*sizeof(int128_t), int128_cmp);
		if( res )
		{
			return i*m + *(res+1);
		}

		y *= am;
		y %= p;
	}

	return 0;
}

int128_t mp_int128_dlog2_bg(int128_t p) { return int128_dlog2_bg(p); }

// 0 : composite
// > 0 : prime
static
int int64_is_prime(int64_t p)
{
	assert( p >= INT64_0 );

	if( p < INT64_2 )
		return 0;

	// 2 is prime
	if( p == INT64_2 )
		return 1;

	// even numbers are composite
	if( ~p & INT64_1 )
		return 0;

	const int64_t sqrt_p = int64_floor_sqrt(p);

	// 3, 5, 7, 9, 11, ...
	for(int64_t factor = INT64_C(3); factor <= sqrt_p; factor += INT64_2)
	{
		if( p % factor == INT64_0 )
			return 0;
	}

	return 1;
}

int mp_int64_is_prime(int64_t p) { return int64_is_prime(p); }

// 0 : composite
// > 0 : prime
static
int int128_is_prime(int128_t p)
{
	assert( p >= INT128_0 );

	if( p < INT128_2 )
		return 0;

	// 2 is prime
	if( p == INT128_2 )
		return 1;

	// even numbers are composite
	if( ~p & INT128_1 )
		return 0;

	const int128_t sqrt_p = int128_floor_sqrt(p);

	// 3, 5, 7, 9, 11, ...
	for(int128_t factor = INT128_C(3); factor <= sqrt_p; factor += INT128_2)
	{
		if( p % factor == INT128_0 )
			return 0;
	}

	return 1;
}

int mp_int128_is_prime(int128_t p) { return int128_is_prime(p); }

static
int int64_is_prime_wheel6(int64_t p)
{
	assert( p >= INT64_0 );

	if( p < 2 )
		return 0;

	if( 2 == p )
		return 1;
	if( ~p & 1 ) // 0 == p % 2
		return 0;

	if( 3 == p )
		return 1;
	if( 0 == p % 3 )
		return 0;

	const int64_t sqrt_p = int64_floor_sqrt(p);

	// iteration 0
	if( 5 == p )
		return 1;
	if( 0 == p % 5 )
		return 0;

	// iteration from 1...
	for(int64_t i = 1; 6*i+1 <= sqrt_p; i++)
	{
		if( 0 == p % (6*i+1) )
			return 0;
		if( 0 == p % (6*i+5) )
			return 0;
	}

	return 1;
}

int mp_int64_is_prime_wheel6(int64_t p) { return int64_is_prime_wheel6(p); }

static
int int128_is_prime_wheel6(int128_t p)
{
	assert( p >= INT128_0 );

	if( p < 2 )
		return 0;

	if( 2 == p )
		return 1;
	if( ~p & 1 ) // 0 == p % 2
		return 0;

	if( 3 == p )
		return 1;
	if( 0 == p % 3 )
		return 0;

	const int128_t sqrt_p = int128_floor_sqrt(p);

	// iteration 0
	if( 5 == p )
		return 1;
	if( 0 == p % 5 )
		return 0;

	// iteration from 1...
	for(int128_t i = 1; 6*i+1 <= sqrt_p; i++)
	{
		if( 0 == p % (6*i+1) )
			return 0;
		if( 0 == p % (6*i+5) )
			return 0;
	}

	return 1;
}

int mp_int128_is_prime_wheel6(int128_t p) { return int128_is_prime_wheel6(p); }

static
int int64_is_prime_wheel30(int64_t p)
{
	assert( p >= INT64_0 );

	if( 0 == p ) return 0;
	if( 1 == p ) return 0;

	const int64_t sqrt_p = int64_floor_sqrt(p);

	if( 2 <= sqrt_p && 0 == p % 2 ) return 0;
	if( 3 <= sqrt_p && 0 == p % 3 ) return 0;
	if( 5 <= sqrt_p && 0 == p % 5 ) return 0;

	for(int64_t i = 0; 30*i+1 <= sqrt_p; i++)
	{
		if( i > 0 && (30*i+ 1) <= sqrt_p && 0 == p % (30*i+ 1) ) return 0;
		if(          (30*i+ 7) <= sqrt_p && 0 == p % (30*i+ 7) ) return 0;
		if(          (30*i+11) <= sqrt_p && 0 == p % (30*i+11) ) return 0;
		if(          (30*i+13) <= sqrt_p && 0 == p % (30*i+13) ) return 0;
		if(          (30*i+17) <= sqrt_p && 0 == p % (30*i+17) ) return 0;
		if(          (30*i+19) <= sqrt_p && 0 == p % (30*i+19) ) return 0;
		if(          (30*i+23) <= sqrt_p && 0 == p % (30*i+23) ) return 0;
		if(          (30*i+29) <= sqrt_p && 0 == p % (30*i+29) ) return 0;
	}

	return 1;
}

int mp_int64_is_prime_wheel30(int64_t p) { return int64_is_prime_wheel30(p); }

static
int int128_is_prime_wheel30(int128_t p)
{
	assert( p >= INT128_0 );

	if( 0 == p ) return 0;
	if( 1 == p ) return 0;

	const int128_t sqrt_p = int128_floor_sqrt(p);

	if( 2 <= sqrt_p && 0 == p % 2 ) return 0;
	if( 3 <= sqrt_p && 0 == p % 3 ) return 0;
	if( 5 <= sqrt_p && 0 == p % 5 ) return 0;

	for(int128_t i = 0; 30*i+1 <= sqrt_p; i++)
	{
		if( i > 0 && (30*i+ 1) <= sqrt_p && 0 == p % (30*i+ 1) ) return 0;
		if(          (30*i+ 7) <= sqrt_p && 0 == p % (30*i+ 7) ) return 0;
		if(          (30*i+11) <= sqrt_p && 0 == p % (30*i+11) ) return 0;
		if(          (30*i+13) <= sqrt_p && 0 == p % (30*i+13) ) return 0;
		if(          (30*i+17) <= sqrt_p && 0 == p % (30*i+17) ) return 0;
		if(          (30*i+19) <= sqrt_p && 0 == p % (30*i+19) ) return 0;
		if(          (30*i+23) <= sqrt_p && 0 == p % (30*i+23) ) return 0;
		if(          (30*i+29) <= sqrt_p && 0 == p % (30*i+29) ) return 0;
	}

	return 1;
}

int mp_int128_is_prime_wheel30(int128_t p) { return int128_is_prime_wheel30(p); }

int message(const char *format, ...)
{
	va_list ap;

	time_t now = time(NULL);
	char buf[26];
	ctime_r(&now, buf);

	buf[strlen(buf)-1] = 0;

	int n = printf("[%s] ", buf);

	va_start(ap, format);
	n += vprintf(format, ap);
	va_end(ap);

	fflush(stdout);

	return n;
}

static
void set_bit(uint8_t *ptr, int i)
{
	ptr[i/8] |= (uint8_t)( 1 << i%8 );
}

static
int get_bit(const uint8_t *ptr, int i)
{
	return ptr[i/8] & 1 << i%8;
}

// 0 : composite
// > 0 : prime
static
int int_is_prime_cached(int p, const uint8_t *primes)
{
	assert( p >= 0 );

	return !get_bit(primes, p);
}

int mp_int_is_prime_cached(int p, const uint8_t *primes) { return int_is_prime_cached(p, primes); }

static
void int64_test_prtest(uint8_t *record, int64_t factor, int exponent_limit, const uint8_t *primes)
{
	// skip M itself
	if( INT64_0 == (factor & (factor+INT64_1)) )
	{
		return;
	}

	// check if the factor \equiv \pm 1 in \pmod 8
	if( (INT64_C(1) != (factor&INT64_C(7))) && (INT64_C(7) != (factor&INT64_C(7))) )
	{
		return;
	}

	// not a prime factor, skip them
	if( !int64_is_prime(factor) )
	{
		return;
	}

	// find M(n)
	int n = (int)mp_int64_dlog2_bg_lim(factor, exponent_limit);

	// check if the exponent is prime
	if( int_is_prime_cached(n, primes) )
	{
		// mark the M(n) as dirty
		set_bit(record, n);

		message(DBG "M(%i) was eliminated by %" PRId64 "!\n", n, factor);
	}
}

void mp_int64_test_prtest(uint8_t *record, int64_t factor, int exponent_limit, const uint8_t *primes) { int64_test_prtest(record, factor, exponent_limit, primes); }

static
void int128_test_prtest(uint8_t *record, int128_t factor, int exponent_limit, const uint8_t *primes)
{
	// skip M itself
	if( INT128_0 == (factor & (factor+INT128_1)) )
	{
		return;
	}

	// check if the factor \equiv \pm 1 in \pmod 8
	if( (INT128_C(1) != (factor&INT128_C(7))) && (INT128_C(7) != (factor&INT128_C(7))) )
	{
		return;
	}

	// not a prime factor, skip them
	if( !int128_is_prime(factor) )
	{
		return;
	}

	// find M(n)
	int n = (int)mp_int128_dlog2_bg_lim(factor, exponent_limit);

	// check if the exponent is prime
	if( int_is_prime_cached(n, primes) )
	{
		// mark the M(n) as dirty
		set_bit(record, n);

		message(DBG "M(%i) was eliminated by %" PRIu64 ":%" PRIu64 "!\n", n, INT128_H64(factor), INT128_L64(factor));
	}
}

void mp_int128_test_prtest(uint8_t *record, int128_t factor, int exponent_limit, const uint8_t *primes) { int128_test_prtest(record, factor, exponent_limit, primes); }

static
void int64_test_direct(uint8_t *record, int64_t factor, int exponent_limit, const uint8_t *primes)
{
	// skip M itself
	if( INT64_0 == (factor & (factor+INT64_1)) )
	{
		return;
	}

	// check if the factor \equiv \pm 1 in \pmod 8
	if( (INT64_C(1) != (factor&INT64_C(7))) && (INT64_C(7) != (factor&INT64_C(7))) )
	{
		return;
	}

	// find M(n)
	int n = (int)mp_int64_dlog2_bg_lim(factor, exponent_limit);

	// check if the exponent is prime
	if( int_is_prime_cached(n, primes) )
	{
		// mark the M(n) as dirty
		set_bit(record, n);

		message(DBG "M(%i) was eliminated by %" PRId64 "!\n", n, factor);
	}
}

void mp_int64_test_direct(uint8_t *record, int64_t factor, int exponent_limit, const uint8_t *primes) { int64_test_direct(record, factor, exponent_limit, primes); }

static
void int128_test_direct(uint8_t *record, int128_t factor, int exponent_limit, const uint8_t *primes)
{
	// skip M itself
	if( INT128_0 == (factor & (factor+INT128_1)) )
	{
		return;
	}

	// check if the factor \equiv \pm 1 in \pmod 8
	if( (INT128_C(1) != (factor&INT128_C(7))) && (INT128_C(7) != (factor&INT128_C(7))) )
	{
		return;
	}

	// find M(n)
	int n = (int)mp_int128_dlog2_bg_lim(factor, exponent_limit);

	// check if the exponent is prime
	if( int_is_prime_cached(n, primes) )
	{
		// mark the M(n) as dirty
		set_bit(record, n);

		message(DBG "M(%i) was eliminated by %" PRIu64 ":%" PRIu64 "!\n", n, INT128_H64(factor), INT128_L64(factor));
	}
}

void mp_int128_test_direct(uint8_t *record, int128_t factor, int exponent_limit, const uint8_t *primes) { int128_test_direct(record, factor, exponent_limit, primes); }
