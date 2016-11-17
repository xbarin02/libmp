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
#include <string.h>
#include <libmp.h>

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

static
void set_bit(char *ptr, int i)
{
	ptr[i/8] |= 1 << i%8;
}

static
int get_bit(const char *ptr, int i)
{
	return ptr[i/8] & 1 << i%8;
}

static
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
	return !get_bit(primes, p);
#endif
}

char *load_prime_table(int exponent_limit)
{
	// check if file exists
	FILE *file = fopen("primes.bits", "r");

	if( NULL == file )
	{
		message(WARN "Cannot load precomputed table of primes.\n");
		return NULL;
	}

	// check if file size corresponds to exponent_limit
	fseek(file, 0L, SEEK_END);
	int detected_exponent_limit = (int)ftell(file)*8;
	rewind(file);

	if( exponent_limit > detected_exponent_limit )
	{
		message(WARN "Prime table of insufficient length.\n");
		fclose(file);
		return NULL;
	}

	// malloc
	char *primes = malloc( (exponent_limit+7)/8 );

	// read the content
	if( (size_t)(exponent_limit+7)/8 != fread(primes, (size_t)1, (size_t)(exponent_limit+7)/8, file) )
	{
		message(ERR "Unable to read precomputed prime table.\n");
		free(primes);
		fclose(file);
		return NULL;
	}

	fclose(file);

	message("Precomputed table of primes was successfully loaded :)\n");

	return primes;
}

void save_prime_table(const char *primes, int exponent_limit)
{
	FILE *file = fopen("primes.bits", "w");

	if( NULL == file )
	{
		message(ERR "Cannot save precomputed table of primes.\n");
	}

	if( (size_t)(exponent_limit+7)/8 != fwrite(primes, (size_t)1, (size_t)(exponent_limit+7)/8, file) )
	{
		message(ERR "Unable to write precomputed prime table.\n");
	}

	fclose(file);
}

// Sieve of Eratosthenes
char *gen_prime_table(int exponent_limit)
{
	message("Creating prime table...\n");

	char *primes = malloc( (exponent_limit+7)/8 );

	if( NULL == primes )
	{
		message(ERR "Unable to allocate memory.\n");
		exit(0);
	}

	// initially: 0 = prime, 1 = composite
	bzero(primes, (exponent_limit+7)/8);

	set_bit(primes, 0);
	set_bit(primes, 1);

	for(int i = 2; i <= ceil_sqrt(exponent_limit); i++)
	{
		if( !get_bit(primes, i) )
		{
			for(int j = i*i; j < exponent_limit; j += i)
			{
				set_bit(primes, j);
			}
		}
	}

	message("The prime table was created.\n");

	return primes;
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
			if( get_bit(record, n) )
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
			if( get_bit(record, n) )
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
	if( (size_t)(exponent_limit+7)/8 != fwrite(record, (size_t)1, (size_t)(exponent_limit+7)/8, record_file) )
	{
		message(ERR "Unable to write into the record :( The file is incomplete!\n");
	}

	fclose(record_file);

	message("The record was saved successfully.\n");
}

struct timespec g_tp0;

void clock_dump(int64_t states)
{
	struct timespec tp1;

	clock_gettime(CLOCK_REALTIME, &tp1);

	int64_t secs_elapsed = (int64_t)tp1.tv_sec - (int64_t)g_tp0.tv_sec;

	float secs_per_state = secs_elapsed/(float)states;

	message("%" PRId64 " seconds elapsed (%f secs per each state).\n",
		secs_elapsed,
		secs_per_state
	);
}

static
const int64_t small_primes[] = {
	  1,
	  2,   3,   5,   7,  11,  13,  17,  19,  23,  29,
	 31,  37,  41,  43,  47,  53,  59,  61,  67,  71,
	 73,  79,  83,  89,  97, 101, 103, 107, 109, 113,
	127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
};

static
int64_t int64_primorial(int n)
{
	assert( (size_t)n+1 < sizeof(small_primes)/sizeof(*small_primes) );

	int64_t p = INT64_1;

	for(int i = 0; i < n; i++)
	{
		p *= small_primes[i+1];
	}

	assert( p > INT64_0 );

	return p;
}

static
int64_t int64_prime(int n)
{
	assert( (size_t)n < sizeof(small_primes)/sizeof(*small_primes) );

	return small_primes[n];
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

static
int random(FILE *random_file)
{
	int r = 0;
	if( (size_t)1 != fread(&r, sizeof(r), (size_t)1, random_file) )
	{
		message(ERR "Unable to get a random value!\n");
	}
	return r;
}

static
int random_difficulty(FILE *random_file)
{
	const int n0 = 8; // inclusive
	const int n1 = 11; // exclusive

	int n;
	do { n = random(random_file); } while( n < 0 );
	n = n0 + n%(n1-n0);

	return n;
}

static
int64_t int64_random_prime_fast(int n, FILE *random_file)
{
	uint64_t r = int64_random(random_file);

	// previous prime
	uint64_t s = INT64_1;

	for(int k = 1; k <= n; k++)
	{
		uint64_t m = int64_prime(k+1);
		uint64_t p = int64_primorial(k);
		uint64_t q = r%m;
		r /= m;

#if 0
		s = (s==INT64_1 || s>=m) ? s : INT64_1;
#endif

		s = q*p + s;
	}

	return s;
}

void sieve(char *record, int exponent_limit, const char *record_path, const char *primes, FILE *random_file)
{
	clock_gettime(CLOCK_REALTIME, &g_tp0);

	message("sieving...\n");

	int64_t states = INT64_0;
	for(;; states++)
	{
		// random difficulty level
		int n = random_difficulty(random_file);

		int64_t factor = int64_random_prime_fast(n, random_file);

		mp_int64_test_direct((uint8_t *)record, factor, exponent_limit, (const uint8_t *)primes);

		if( g_term )
		{
			// exit the program
			break;
		}

		if( g_save )
		{
			// save the record and state...
			record_save(record, exponent_limit, record_path);

			g_save = 0;
		}

		if( g_info )
		{
			message("%" PRId64 " random states tested so far.\n", states);

			// gather and print a progress overview
			summary(record, exponent_limit, primes);

			clock_dump(states);

			g_info = 0;
		}
	}

	// save the record and state
	record_save(record, exponent_limit, record_path);
	clock_dump(states);

	// gather and print a progress overview
	summary(record, exponent_limit, primes);
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
		record = malloc( (size_t)(*p_exponent_limit+7)/8 );
		if( NULL == record )
		{
			message(ERR "Unable to allocate memory :( %zu bytes requested.\n", (size_t)(*p_exponent_limit+7)/8);
			exit(0);
		}

		bzero(record, (size_t)(*p_exponent_limit+7)/8);

		message("There is no record. Created a new record of %i exponents in size (%i MiB in memory, %i MiB in file)!\n",
			(*p_exponent_limit),
			(*p_exponent_limit + (1<<23) - 1)>>23,
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
		record = malloc( (size_t)(*p_exponent_limit+7)/8 );
		if( NULL == record )
		{
			message(ERR "Unable to allocate memory :( %zu bytes requested.\n", (size_t)(*p_exponent_limit+7)/8);
			exit(0);
		}

		// all zeros implicitly
		bzero(record, (size_t)(*p_exponent_limit+7)/8);

		// loop over the record
		if( (size_t)(*p_exponent_limit+7)/8 != fread(record, (size_t)1, (size_t)(*p_exponent_limit+7)/8, record_file) )
		{
			message(ERR "Unable to read from the record :( The file may be corrupted!\n");
		}

		message("Loaded an existing record of %i exponents in size (%i MiB in memory, %i MiB in file)!\n",
			(*p_exponent_limit),
			(*p_exponent_limit + (1<<23) - 1)>>23,
			(*p_exponent_limit + (1<<23) - 1)>>23
		);

		fclose(record_file);
	}

	return record;
}

int main(int argc, char *argv[])
{
	message("%s: Sieve of Mersenne exponents, 64-bit random test\n", argv[0]);

	// default options
	unsigned int timeout = 0; // no limit
	int exponent_limit = -1;
	const char *record_path = "record.bits";

	// parse command-line options
	for(int opt; (opt = getopt(argc, argv, "t:h:")) != -1;)
	{
		switch(opt)
		{
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
	char *primes = load_prime_table(exponent_limit);
	if( NULL == primes )
	{
		primes = gen_prime_table(exponent_limit);
		save_prime_table(primes, exponent_limit);
	}

	FILE *random_file = fopen("/dev/urandom", "r");
	if( NULL == random_file )
	{
		message(ERR "Unable to open a pseudorandom number generator.\n");
		exit(0);
	}

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
	sieve(record, exponent_limit, record_path, primes, random_file);

	fclose(random_file);

	free(record);
	free(primes);

	message("The program has finished successfully.\n");

	return 0;
}
