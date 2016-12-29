#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <libmp.h>

struct timespec g_tp0, g_tp1;

void clock_reset()
{
	clock_gettime(CLOCK_REALTIME, &g_tp0);
}

void clock_dump(int64_t states)
{
	clock_gettime(CLOCK_REALTIME, &g_tp1);

	int64_t secs_elapsed = (int64_t)g_tp1.tv_sec - (int64_t)g_tp0.tv_sec;
	float secs_per_state = secs_elapsed/(float)states*1000;

	printf("\t\t%" PRId64 " seconds elapsed (%f msecs per each state).\n\n",
		secs_elapsed,
		secs_per_state
	);
}

#define TEST128(func) \
do { \
	printf("\t" #func "\n"); \
	clock_reset(); \
	for(int128_t p = (INT128_1<<bit_level) + 1; p < (INT128_1<<(bit_level+1)); p++) \
	{ \
		for(int128_t a = 0; a < p; a++) \
		{ \
			func(p, a, a); \
		} \
	} \
	clock_dump(INT64_1<<2*bit_level); \
} while(0)


int main()
{
	// for each range
	for(int bit_level = 0; bit_level < 128; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

		TEST128(mp_int128_dmul);
	}

	return 0;
}
