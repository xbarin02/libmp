#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include <signal.h>
#include <assert.h>
#include <strings.h>
#include <time.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#define ERR "ERROR: "
#define WARN "WARNING: "

int message(const char *format, ...)
{
	va_list ap;

	time_t now = time(NULL);
	char buf[26];
	ctime_r(&now, buf);

	buf[strlen(buf)-1] = 0;

	int n = printf("[%s] ", buf);

	va_start(ap, format);
	n += vprintf(format, ap);
	va_end(ap);

	return n;
}

int g_term = 0;
int g_info = 0;
int g_save = 0;

void sighandler_int(int signum)
{
	(void)signum;
	exit(0);
}

void sighandler_alrm(int signum)
{
	(void)signum;
	g_term = 1;
}

void sighandler_term(int signum)
{
	(void)signum;
	g_term = 1;

	// set the signal handler again
	signal(SIGTERM, sighandler_term);
}

void sighandler_usr1(int signum)
{
	(void)signum;
	g_info = 1;

	// set the signal handler again
	signal(SIGUSR1, sighandler_usr1);
}

void sighandler_usr2(int signum)
{
	(void)signum;
	g_save = 1;

	// set the signal handler again
	signal(SIGUSR2, sighandler_usr2);
}

#define INT64_0 INT64_C(0)
#define INT64_1 INT64_C(1)

int ceil_sqrt(int n)
{
	assert( n > 0 );

	int x = n;
	int y = 0;

	while(x != y)
	{
		y = x;
		x = (x + (n + x - 1)/x + 1) >> 1;
	}

	return x;
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

int is_prime(int p, const char *primes)
{
	assert( p >= 0 );
#if 0

	if( p < 2 )
		return 0;

	if( p == 2 )
		// prime
		return 1;

	const int sqrt_p = ceil_sqrt(p);

	for(int factor = 2; factor <= sqrt_p; factor++)
	{
		if( p % factor == 0 )
			// composite
			return 0;
	}

	// prime
	return 1;
#else
	return !primes[p];
#endif
}

// Sieve of Eratosthenes
char *gen_prime_table(int exponent_limit)
{
	message("Creating prime table...\n");

	char *primes = malloc(exponent_limit);

	if( NULL == primes )
	{
		message(ERR "Unable to allocate memory.\n");
		exit(0);
	}

	// initially: 0 = prime, 1 = composite
	bzero(primes, exponent_limit);

	primes[0] = 1;
	primes[1] = 1;

	for(int i = 2; i <= ceil_sqrt(exponent_limit); i++)
	{
		if( !primes[i] )
		{
			for(int j = i*i; j < exponent_limit; j += i)
			{
				primes[j] = 1;
			}
		}
	}

	message("The prime table was created.\n");

	return primes;
}

int int64_is_prime(int64_t p)
{
	assert( p >= INT64_0 );

	if( p < INT64_C(2) )
		return 0;

	if( p == INT64_C(2) )
		// prime
		return 1;

	const int64_t sqrt_p = int64_ceil_sqrt(p);

	for(int64_t factor = INT64_C(2); factor <= sqrt_p; factor++)
	{
		if( p % factor == INT64_0 )
			// composite
			return 0;
	}

	// prime
	return 1;
}

static
int64_t int64_min(int64_t a, int64_t b)
{
	return a < b ? a : b;
}

int dlog2(int64_t p, int exponent_limit)
{
	int k1 = (int)int64_min(p, (int64_t)exponent_limit) - 1;
	
	// b^k
	int64_t m = INT64_1;

	int k = 0;

	while( k < k1 )
	{
		// b^k = b^k * b = b^(k+1)
		m <<= 1;
		m %= p;

		// k = k + 1
		k++;

		// b^k == g
		if( INT64_1 == m )
			return k;
	}
	// FIXME: m=0 never become 1 (although, probably not valid for p : PRIME)

	// fallback
	return 0;
}

void test(char *record, int64_t factor, int exponent_limit, const char *primes)
{
	if( INT64_0 == (factor & (factor+INT64_1)) )
	{
		// skip M itself
		return;
	}

	if( !int64_is_prime(factor) )
	{
		// not a prime factor, skip them
		return;
	}

	// find M(n)
	int n = dlog2(factor, exponent_limit);

	// check if the exponent is prime
	if( is_prime(n, primes) )
	{
		// mark the M(n) as dirty
		record[n] = 1;
	}
}

void summary(const char *record, int exponent_limit, const char *primes)
{
	int prime_total = 0; // prime exponents in total, Mersenne numbers
	int prime_eliminated = 0; // dirty exponents, factor found
	int prime_candidates = 0; // clean exponents, possible Mersenne primes
	int composite_total = 0; // composite exponents
	int composite_dirty = 0; // dirty composite exponents, should be zero
	int first_candidate = 0;
	int biggest_eliminated = 0;

	for(int n = 1; n < exponent_limit; n++)
	{
		if( is_prime(n, primes) )
		{
			prime_total++;

			// if dirty
			if( record[n] )
			{
				prime_eliminated++;

				biggest_eliminated = n;
			}
			else
			{
				prime_candidates++;
				
				if(!first_candidate)
				{
					first_candidate = n;
				}
			}
		}
		else
		{
			composite_total++;

			// if dirty
			if( record[n] )
			{
				composite_dirty++;
			}
		}
	}

	message("Summary: %i prime exponents (%i candidates + %i eliminated) and %i composite exponents (%i dirty) out of %i exponents in total. The first candidate is M(%i). The biggest eliminated is M(%i).\n",
		prime_total, prime_candidates, prime_eliminated, composite_total, composite_dirty, exponent_limit-1, first_candidate, biggest_eliminated
	);
}

// save the record
void record_save(char *record, int exponent_limit, const char *record_path)
{
	message("Saving the record to '%s'...\n", record_path);

	// remove backup copy
	char backup_path[4096];
	sprintf(backup_path, "%s.bak", record_path);
	unlink(backup_path);

	// rename a previous record
	rename(record_path, backup_path);

	// create new record
	FILE *record_file = fopen(record_path, "w");
	if( NULL == record_file )
	{
		message(ERR "Unable to save the record :(\n");
		return;
	}

	// loop over the record
	for(int byte = 0; byte < (exponent_limit+7)/8; byte++)
	{
		uint8_t b = 0;

		for(int bit = 0; bit < 8 && bit+byte*8 < exponent_limit; bit++)
		{
			if( record[bit+byte*8] )
			{
				b |= 1<<bit;
			}
		}

		if( (size_t)1 != fwrite(&b, (size_t)1, (size_t)1, record_file) )
		{
			message(ERR "Unable to write into the record :( The file is incomplete!\n");
			break;
		}
	}

	fclose(record_file);

	message("The record was saved successfully.\n");
}

void state_save(int64_t state)
{
	FILE *state_file = fopen("sieve.state", "wb");
	if( NULL == state_file )
	{
		message(ERR "Unable to save the state :(\n");
		return;
	}

	if( fprintf(state_file, "%" PRId64, state) < 0 )
	{
		message(ERR "Unable to write the state :(\n");
	}

	fclose(state_file);

	message("The state (%" PRId64 ") was saved successfully.\n", state);
}

struct timespec g_tp0;

void clock_dump(int64_t init_state, int64_t state)
{
	struct timespec tp1;

	clock_gettime(CLOCK_REALTIME, &tp1);

	int64_t secs_elapsed = (int64_t)tp1.tv_sec - (int64_t)g_tp0.tv_sec;

	message("%" PRId64 " seconds elapsed (%f secs per each state).\n",
		secs_elapsed,
		secs_elapsed/(float)(state - init_state)
	);
}

void sieve(char *record, int64_t init_state, int exponent_limit, const char *record_path, const char *primes)
{
	// for 64 bits: 1 + 60 + 3
	int64_t max_state = (INT64_1<<60) - INT64_1;

	clock_gettime(CLOCK_REALTIME, &g_tp0);

	for(int64_t state = init_state; state <= max_state; state++)
	{
		int64_t factor1 = state*INT64_C(8) + INT64_1;
		int64_t factor7 = state*INT64_C(8) - INT64_1;

		if( INT64_0 == (factor7 & (factor7 + INT64_1)) )
		{
			message("Entering bit level %i...\n", __builtin_popcountll(factor7));
		}

		test(record, factor1, exponent_limit, primes);
		test(record, factor7, exponent_limit, primes);

		if( g_term )
		{
			max_state = state;

			// exit the program
			break;
		}

		if( g_save )
		{
			// save the record and state...
			record_save(record, exponent_limit, record_path);
			state_save(state+INT64_1);

			g_save = 0;
		}

		if( g_info )
		{
			message("Current state is %" PRId64 ".\n", state);

			// gather and print a progress overview
			summary(record, exponent_limit, primes);

			clock_dump(init_state, state);

			g_info = 0;
		}
	}

	// save the record and state
	record_save(record, exponent_limit, record_path);
	state_save(max_state+INT64_1);
	clock_dump(init_state, max_state);
}

// load the record
char *record_load(int *p_exponent_limit, const char *record_path)
{
	char *record;
	message("Loading the record from '%s'...\n", record_path);
	FILE *record_file = fopen(record_path, "rb");
	if( NULL == record_file )
	{
		// exponent_limit is unset, use default
		if( -1 == *p_exponent_limit )
		{
			*p_exponent_limit = 256*1024*1024; // up to 268M
		}

		// no record file, start a new test
		record = malloc((size_t)*p_exponent_limit);
		if( NULL == record )
		{
			message(ERR "Unable to allocate memory :( %zu bytes requested.\n", (size_t)*p_exponent_limit);
			exit(0);
		}

		bzero(record, (size_t)*p_exponent_limit);

		message("There is no record. Created a new record of %i exponents in size (%i MiB in memory, %i MiB in file)!\n",
			(*p_exponent_limit),
			(*p_exponent_limit + (1<<20) - 1)>>20,
			(*p_exponent_limit + (1<<23) - 1)>>23
		);
	}
	else
	{
		// detect the highest exponent from the existing record using fstat
		fseek(record_file, 0L, SEEK_END);
		long detected_exponent_limit = ftell(record_file) * 8;
		rewind(record_file);

		// exponent_limit is unset, use fstat
		if( -1 == *p_exponent_limit )
		{
			message("The highest exponent in the record is %li (detected).\n", detected_exponent_limit);
			*p_exponent_limit = detected_exponent_limit;
		}

		// if exponent_limit <> fstat, print a warning
		if( *p_exponent_limit < detected_exponent_limit )
		{
			message(WARN "Forced exponent limit (%i) is is less than the detected one (%li)! The record will be truncated.\n",
				*p_exponent_limit, detected_exponent_limit
			);
		}
		if( *p_exponent_limit > detected_exponent_limit )
		{
			message(WARN "Forced exponent limit (%i) is is greater than the detected one (%li)! The record will be extended.\n",
				*p_exponent_limit, detected_exponent_limit
			);
		}

		// read the record content
		record = malloc((size_t)*p_exponent_limit);
		if( NULL == record )
		{
			message(ERR "Unable to allocate memory :( %zu bytes requested.\n", (size_t)*p_exponent_limit);
			exit(0);
		}

		// all zeros implicitly
		bzero(record, (size_t)*p_exponent_limit);

		// loop over the record
		for(int byte = 0; byte < (*p_exponent_limit+7)/8; byte++)
		{
			uint8_t b = 0;
			
			if( (size_t)1 != fread(&b, (size_t)1, (size_t)1, record_file) )
			{
				message(ERR "Unable to read from the record :( The file may be corrupted!\n");
				break;
			}

			for(int bit = 0; bit < 8 && bit+byte*8 < *p_exponent_limit; bit++)
			{
				if( b & 1<<bit )
				{
					record[bit+byte*8] = 1;
				}
			}
		}

		message("Loaded an existing record of %i exponents in size (%i MiB in memory, %i MiB in file)!\n",
			(*p_exponent_limit),
			(*p_exponent_limit + (1<<20) - 1)>>20,
			(*p_exponent_limit + (1<<23) - 1)>>23
		);

		fclose(record_file);
	}

	return record;
}

// load the state
void state_load(int64_t *p_init_state)
{
	FILE *state_file = fopen("sieve.state", "rb");
	if( NULL == state_file )
	{
		// init_state is unset, use default
		if( INT64_0 == *p_init_state )
		{
			*p_init_state = INT64_1;
		}

		message("There is no saved state, starting from %" PRId64 "...\n", *p_init_state);
	}
	else
	{
		if( INT64_0 != *p_init_state )
		{
			message(WARN "Overwriting the saved state, now starting from %" PRId64 "...\n", *p_init_state);
		}
		else
		{
			// load sieve.state (or fallback to default)
			if( fscanf(state_file, "%" PRId64, p_init_state) < 0 )
			{
				message(ERR "Unable to read the state :( Falling back to the default one.\n");
				*p_init_state = INT64_1;
			}

			message("Starting from the previous state %" PRId64 "...\n", *p_init_state);
		}

		fclose(state_file);
	}
}

int main(int argc, char *argv[])
{
	message("%s: Sieve of Mersenne exponents, 64-bit sequential test\n", argv[0]);

	// default options
	int64_t init_state = INT64_0;
	unsigned int timeout = 0; // no limit
	int exponent_limit = -1;
	const char *record_path = "record.bits";

	// parse command-line options
	for(int opt; (opt = getopt(argc, argv, "s:t:h:")) != -1;)
	{
		switch(opt)
		{
			// -s STATE : start test at the exact state (more precise than the bit level)
			case 's':
				init_state = atoll(optarg);
				if( init_state < INT64_1 )
				{
					message(WARN "Invalid state, keeping the default one!\n");
					init_state = INT64_0;
				}
				break;
			// -t SECS : terminate test after given time limit (in seconds)
			case 't':
				timeout = atoi(optarg);
				break;
			// -h EXP : highest exponent in the record
			case 'h':
				exponent_limit = atoi(optarg);
				if( exponent_limit < 2 )
				{
					message(WARN "Invalid exponent, keeping the default one!\n");
					exponent_limit = -1;
				}
				break;
			default :
				message(WARN "Unknown option :( Read the source code!\n");
		}
	}

	// argument after options
	if(optind < argc)
	{
		// sieve-64 [OPTIONS] [record.bits]
		record_path = argv[optind++];

		if(optind < argc)
		{
			message(WARN "Too much options :( Read the source code!\n");
		}
	}

	// load the record
	char *record = record_load(&exponent_limit, record_path);

	// create prime table
	char *primes = gen_prime_table(exponent_limit);

	// load the state
	state_load(&init_state);

	// set SIGINT, SIGALRM, SIGUSR1, SIGUSR2, and SIGTERM signal handlers
	signal(SIGINT,  sighandler_int); // exit immediately
	signal(SIGALRM, sighandler_alrm); // save the record and state, exit
	signal(SIGTERM, sighandler_term); // save the record and state, exit
	signal(SIGUSR1, sighandler_usr1); // print progress overview
	signal(SIGUSR2, sighandler_usr2); // save the record and state

	// set the alarm
	if( timeout > 0 )
	{
		message("Setting an alarm to be delivered in %i seconds...\n", timeout);
		alarm(timeout);
	}

	// start a loop
	sieve(record, init_state, exponent_limit, record_path, primes);

	free(record);

	message("The program has finished successfully.\n");

	return 0;
}
