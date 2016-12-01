#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <libmp.h>

int main()
{
	// for each range
	for(int bit_level = 0; bit_level < 64; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

		// for each modulus in [ 2^bit_level .. 2^(bit_level+1) )
		for(int64_t n = (INT64_1<<bit_level); n < (INT64_1<<(bit_level+1)); n++)
		{
			for(int64_t a = 0; a < n; a++)
			{
				for(int64_t b = 1; b < n; b++)
				{
					int64_t c = a * b % n;
					int64_t q = mp_int64_divide(c, b, n);

					assert( c == q * b % n );
				}
			}
		}
	}

	return 0;
}
