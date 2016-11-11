#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>

#define INT64_0 INT64_C(0)
#define INT64_1 INT64_C(1)
#define INT64_2 INT64_C(2)

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

int64_t dlog2_lsb(int64_t p)
{
	assert( p > INT64_0 );

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

int main(int argc, char *argv[])
{
	switch(argc)
	{
		case 1:
			printf("Usage:\n\t%s [f]\t.. returns dlog2(f)\n\t%s [f] [r]\t.. returns residue of r=dlog2(f)\n", *argv, *argv);
			break;
		case 2:
		{
			int64_t f = atol(argv[1]);
			printf("dlog2(%" PRId64 ") = %" PRId64 " (LSB)\n", f, dlog2_lsb(f));
			printf("dlog2(%" PRId64 ") = %" PRId64 " (MSB)\n", f, dlog2_msb(f));
		}
			break;
		case 3:
		{
			int64_t f = atol(argv[1]);
			int64_t r = atol(argv[2]);
			printf("alpha(dlog2(%" PRId64 ")=%" PRId64 ") = %" PRId64 " (LSB)\n", f, r, dlog2_r_lsb(f, r));
			printf("alpha(dlog2(%" PRId64 ")=%" PRId64 ") = %" PRId64 " (MSB)\n", f, r, dlog2_r_msb(f, r));
		}
			break;
		default:
			return main(1, argv);
	}

	return 0;
}
