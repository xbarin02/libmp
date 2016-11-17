#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <libmp.h>
#include <time.h>

struct timespec g_tp0, g_tp1;

void clock_reset()
{
	clock_gettime(CLOCK_REALTIME, &g_tp0);
}

void clock_dump(int64_t states)
{
	clock_gettime(CLOCK_REALTIME, &g_tp1);

	int64_t secs_elapsed = (int64_t)g_tp1.tv_sec - (int64_t)g_tp0.tv_sec;
	float secs_per_state = secs_elapsed/(float)states;

	printf("\t\t%" PRId64 " seconds elapsed (%f secs per each state).\n\n",
		secs_elapsed,
		secs_per_state
	);
}

int main()
{
	// for each range
	for(int bit_level = 0; bit_level < 64; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

		// mp_int64_is_prime
		{
			printf("\tmp_int64_is_prime\n");
			clock_reset();

			// for each factor in [ 2^bit_level .. 2^(bit_level+1)-1 )
			for(int64_t f = (INT64_1<<bit_level); f < (INT64_1<<(bit_level+1)); f++)
			{
				mp_int64_is_prime(f);
			}

			clock_dump(INT64_1<<bit_level);
		}

		// mp_int128_is_prime
		{
			printf("\tmp_int128_is_prime\n");
			clock_reset();

			// for each factor in [ 2^bit_level .. 2^(bit_level+1)-1 )
			for(int64_t f = (INT64_1<<bit_level); f < (INT64_1<<(bit_level+1)); f++)
			{
				mp_int128_is_prime(f);
			}

			clock_dump(INT64_1<<bit_level);
		}
	}

	return 0;
}
