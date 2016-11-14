#include "libmp.h"

#include <stdint.h>
#include <assert.h>

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
