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

// https://en.wikipedia.org/wiki/Baby-step_giant-step
int64_t dlog2_bga(int64_t p)
{
	int64_t m = int64_ceil_sqrt(p);

	int64_t tab[m];

	int64_t aj = INT64_1;
	for(int64_t j = INT64_0; j < m; j++)
	{
		tab[j] = aj;
		aj <<= 1;
		if( aj >= p )
			aj -= p;
	}

	int64_t am = dlog2_r_lsb(p, m);

	int64_t y = INT64_1;
	for(int64_t i = INT64_0; i < m; i++)
	{
		// because 2^0 == 1
		if( i > INT64_0 )
		{
			for(int64_t j = INT64_0; j < m; j++)
			{
				if( y == tab[j] )
				{
					return i*m + j;
				}
			}
		}
		y *= am;
		y %= p;
	}

	assert( "failure" );
	return 0;
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
			printf("dlog2(%" PRId64 ") = %" PRId64 " (BGA)\n", f, dlog2_bga(f));
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
