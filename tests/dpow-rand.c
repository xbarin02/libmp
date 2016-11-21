#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
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

	// for each bit level
	for(int bit_level = 0; bit_level < 64; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

		// random ODD factor in [ 2^bit_level .. 2^(bit_level+1) )
		int64_t f = ( (uint64_t)int64_random(random_file) & ( (INT64_1<<(bit_level+1)) - INT64_1 ) ) | INT64_1;

		// debugging purposes
		printf("\t factor = %li\n", f);

		// (mod 1) is problematic: should 2^1 (mod 1) be zero?
		if( f > 1 )
		{
			// random power < 2*f
			int64_t k = (uint64_t)int64_random(random_file) % (2*f);

			// debugging purposes
			printf("\t power = %li\n", k);

			{
				// (-)
				const int64_t r_mn = dpow2_mn(f, k);
				assert( r_mn == mp_int64_dpow2_mn(f, k) );
				assert( r_mn == mp_int128_dpow2_mn(f, k) );

				// (+)
				const int64_t r_pl = dpow2_pl(f, k);
				assert( r_pl == mp_int64_dpow2_pl(f, k) );
				assert( r_pl == mp_int128_dpow2_pl(f, k) );
				if( r_pl != mp_int64_dpow2_pl_log(f, k) )
				{
					printf("\t\t (+) reference = %li\n\t\t mp_int64_dpow2_pl_log = %li\n", r_pl, mp_int64_dpow2_pl_log(f, k));
				}
				assert( r_pl == mp_int64_dpow2_pl_log(f, k) );
				assert( r_pl == mp_int128_dpow2_pl_log(f, k) );
			}
		}
	}

	fclose(random_file);

	return 0;
}
