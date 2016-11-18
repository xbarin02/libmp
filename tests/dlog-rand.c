#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
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

static
int64_t int64_random(FILE *random_file)
{
	int64_t r = 0;

	if( (size_t)1 != fread(&r, sizeof(r), (size_t)1, random_file) )
	{
		message(ERR "Unable to get a random value!\n");
	}

	return r;
}

int main()
{
	FILE *random_file = fopen("/dev/urandom", "r");
	if( NULL == random_file )
	{
		message(ERR "Unable to open a pseudorandom number generator.\n");
		exit(0);
	}

	// for each range
	for(int bit_level = 0; bit_level < 64; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

		// random ODD factor in [ 2^bit_level .. 2^(bit_level+1) )
		int64_t f = ( int64_random(random_file) & ( (INT64_1<<(bit_level+1)) - INT64_1 ) ) | INT64_1;

		// debugging purposes
		printf("\t factor = %li\n", f);

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

			// 128-bit tests
			assert( r == mp_int128_dlog2_mn(f) );
			assert( r == mp_int128_dlog2_pl(f) );
			assert( r == mp_int128_dlog2_bg(f) );
			assert( r == mp_int128_dlog2_mn_lim(f, INT128_1<<126) );
			assert( r == mp_int128_dlog2_pl_lim(f, INT128_1<<126) );
			assert( r == mp_int128_dlog2_bg_lim(f, INT128_1<<126) );
		}
	}

	fclose(random_file);

	return 0;
}
