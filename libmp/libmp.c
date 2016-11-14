#include "libmp.h"

#include <stdint.h>
#include <assert.h>
#include <stddef.h>

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
	return (*(const int64_t *)p2) - (*(const int64_t *)p1);
}

static
int int128_cmp(const void *p1, const void *p2)
{
	return (*(const int128_t *)p2) - (*(const int128_t *)p1);
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
