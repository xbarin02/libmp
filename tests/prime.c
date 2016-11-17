#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <libmp.h>

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

// reference
static
int is_prime(int64_t p)
{
	assert( p >= INT64_0 );

	if( p < INT64_C(2) )
		return 0;

	if( p == INT64_C(2) )
		return 1;

	const int64_t sqrt_p = int64_ceil_sqrt(p);

	for(int64_t factor = INT64_C(2); factor <= sqrt_p; factor++)
	{
		if( p % factor == INT64_0 )
			return 0;
	}

	return 1;
}

int main()
{
	// for each range
	for(int bit_level = 0; bit_level < 64; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

		// for each factor in [ 2^bit_level .. 2^(bit_level+1) )
		for(int64_t f = (INT64_1<<bit_level); f < (INT64_1<<(bit_level+1)); f++)
		{
			const int r = is_prime(f);

			// TEST: mp_int64_is_prime
			assert( r == mp_int64_is_prime(f) );
			// TEST: mp_int128_is_prime
			assert( r == mp_int128_is_prime(f) );
		}
	}

	return 0;
}
