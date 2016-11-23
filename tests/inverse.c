#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <libmp.h>

int main()
{
	// for each bit level
	for(int bit_level = 0; bit_level < 64; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

		// for each ODD modulus (factor)
		for(int64_t n = (INT64_1<<bit_level) + 1; n < (INT64_1<<(bit_level+1)); n+=2)
		{
			// for each element x < modulus
			for(int64_t x = 1; x < n; x++)
			{
				// compute inverse x
				{
					int64_t inv_x = mp_int64_inverse(x, n);

					// not invertible
					if( 0 == inv_x )
					{
						// The multiplicative inverse of a modulo m exists if and only if a and m are coprime (i.e., if gcd(a, m) = 1).
						assert( 1 != mp_int64_gcd(x, n) );

						continue;
					}

					assert( 1 == (x * inv_x) % n );
				}

				{
					int128_t inv_x = mp_int128_inverse(x, n);

					// not invertible
					if( 0 == inv_x )
					{
						// The multiplicative inverse of a modulo m exists if and only if a and m are coprime (i.e., if gcd(a, m) = 1).
						assert( 1 != mp_int128_gcd(x, n) );

						continue;
					}

					assert( 1 == (x * inv_x) % n );
				}
			}
		}
	}

	return 0;
}
