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
	float secs_per_state = secs_elapsed/(float)states*1000;

	printf("\t\t%" PRId64 " seconds elapsed (%f msecs per each state).\n\n",
		secs_elapsed,
		secs_per_state
	);
}

#define TEST(func) \
do { \
	printf("\t" #func "\n"); \
	clock_reset(); \
	for(int64_t f = (INT64_1<<bit_level) + 1; f < (INT64_1<<(bit_level+1)); f += 2) \
	{ \
		for(int64_t k = 0; k < 2*f; k++) \
		{ \
			func(f, k); \
		} \
	} \
	clock_dump(INT64_1<<2*bit_level); \
} while(0)


int main()
{
	// for each range
	for(int bit_level = 0; bit_level < 64; bit_level++)
	{
		printf("testing the bit level %i...\n", bit_level);

		TEST(mp_int64_dpow2_mn);

		TEST(mp_int128_dpow2_mn);

		TEST(mp_int64_dpow2_pl);

		TEST(mp_int128_dpow2_pl);
	}

	return 0;
}
