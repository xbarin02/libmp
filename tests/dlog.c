#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <libmp.h>

// naive implementation as the reference one
static
int64_t dlog2(int64_t p)
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

int main()
{
	// for each range
	for(int bit_level = 0; bit_level < 64; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

		// for each ODD factor in [ 2^bit_level .. 2^(bit_level+1) )
		for(int64_t f = (INT64_1<<bit_level) + 1; f < (INT64_1<<(bit_level+1)); f += 2)
		{
			// reference
			int64_t r = dlog2(f);

			// 64-bit tests
			assert( r == mp_int64_dlog2_mn(f) );
			assert( r == mp_int64_dlog2_pl(f) );
			assert( r == mp_int64_dlog2_bg(f) );
			assert( r == mp_int64_dlog2_mn_lim(f, INT64_1<<62) );
			assert( r == mp_int64_dlog2_pl_lim(f, INT64_1<<62) );
			assert( r == mp_int64_dlog2_bg_lim(f, INT64_1<<62) );
			assert( !mp_int64_is_prime(f) || r == mp_int64_element2_order(f) );

			// 128-bit tests
			assert( r == mp_int128_dlog2_mn(f) );
			assert( r == mp_int128_dlog2_pl(f) );
			assert( r == mp_int128_dlog2_bg(f) );
			assert( r == mp_int128_dlog2_mn_lim(f, INT128_1<<126) );
			assert( r == mp_int128_dlog2_pl_lim(f, INT128_1<<126) );
			assert( r == mp_int128_dlog2_bg_lim(f, INT128_1<<126) );
			assert( !mp_int64_is_prime(f) || r == mp_int128_element2_order(f) );
		}
	}

	return 0;
}
