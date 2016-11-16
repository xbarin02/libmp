#include "libmp.h"

#include <stdint.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

// 2^(+k) (mod p) [MSB, slower]
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

// 2^(+k) (mod p) [MSB, slower]
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

// 2^(-k) (mod p) [LSB, faster]
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

// 2^(-k) (mod p) [LSB, faster]
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
#else
	int64_t x = (*(const int64_t *)p2) - (*(const int64_t *)p1);
	return (x < 0) ? -1 : (x > 0);
#endif
}

static
int int128_cmp(const void *p1, const void *p2)
{
#if 0
	// BUG: overflows when casted to the int
	return (int)( (*(const int128_t *)p2) - (*(const int128_t *)p1) );
#else
	int128_t x = (*(const int128_t *)p2) - (*(const int128_t *)p1);
	return (x < 0) ? -1 : (x > 0);
#endif
}

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

// a*b (mod p)
static
int64_t int64_dmul_int128(int64_t p, int64_t a, int64_t b)
{
	return (int64_t)( ( (int128_t)a * (int128_t)b ) % (int128_t)p );
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

// 0 : composite
// > 0 : prime
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

	const int64_t sqrt_p = int64_ceil_sqrt(p);

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

	const int128_t sqrt_p = int128_ceil_sqrt(p);

	// 3, 5, 7, 9, 11, ...
	for(int128_t factor = INT128_C(3); factor <= sqrt_p; factor += INT128_2)
	{
		if( p % factor == INT128_0 )
			return 0;
	}

	return 1;
}

int mp_int128_is_prime(int128_t p) { return int128_is_prime(p); }

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
