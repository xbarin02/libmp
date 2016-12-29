#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <libmp.h>
#include <stdlib.h>

#define TEST128(func) \
do { \
	printf("\t" #func "\n"); \
	for(uint128_t p = (INT128_1<<bit_level) + 1; p < ((uint128_t)INT128_1<<(bit_level+1)); p++) \
	{ \
		for(uint128_t a = 0; a < p; a++) \
		{ \
			for(uint128_t b = 0; b < p; b++) \
			{ \
				for(uint128_t c = 0; c < p; c++) \
				{ \
					assert( func(p, (a+b)%p, c) == ( func(p, a, c) + func(p, b, c) ) % p ); \
				} \
			} \
		} \
	} \
} while(0)

#define RAND128(func) \
do { \
	printf("\t" #func "\n"); \
	for(uint128_t p = (INT128_1<<bit_level) + 1; p < ((uint128_t)INT128_1<<(bit_level+1)); p += (bit_level-12>0)?((uint128_t)INT128_1<<(bit_level-12)):((uint128_t)1)) \
	{ \
		uint128_t a = rand() % p; \
		uint128_t b = rand() % p; \
		uint128_t c = rand() % p; \
		assert( func(p, (a+b)%p, c) == ( func(p, a, c) + func(p, b, c) ) % p ); \
	} \
} while(0)

int main()
{
	// for each range
	for(int bit_level = 0; bit_level < 128; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

// 		TEST128(mp_int128_dmul);
		RAND128(mp_int128_dmul);
	}

	return 0;
}
