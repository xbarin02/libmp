#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <libmp.h>

// reference (+)
static
int64_t dpow2_pl(int64_t p, int64_t K)
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

// reference (-)
static
int64_t dpow2_mn(int64_t p, int64_t K)
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

int main()
{
	// for each range
	for(int bit_level = 0; bit_level < 64; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

		// for each ODD factor in [ 2^bit_level .. 2^(bit_level+1) )
		for(int64_t f = (INT64_1<<bit_level) + 1; f < (INT64_1<<(bit_level+1)); f += 2)
		{
			// for each power
			for(int64_t k = 0; k < 2*f; k++)
			{
				// (-)
				const int64_t r_mn = dpow2_mn(f, k);
				assert( r_mn == mp_int64_dpow2_mn(f, k) );
				assert( r_mn == mp_int128_dpow2_mn(f, k) );
				assert( r_mn == mp_int64_dpow2_mn_log(f, k) );
				assert( r_mn == mp_int128_dpow2_mn_log(f, k) );

				// (+)
				const int64_t r_pl = dpow2_pl(f, k);
				assert( r_pl == mp_int64_dpow2_pl(f, k) );
				assert( r_pl == mp_int128_dpow2_pl(f, k) );
				assert( r_pl == mp_int64_dpow2_pl_log(f, k) );
				assert( r_pl == mp_int128_dpow2_pl_log(f, k) );
			}
		}
	}

	return 0;
}
