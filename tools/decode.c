#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include <libmp.h>

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
int is_prime(int p, const char *primes)
{
	assert( p >= 0 );

	return !get_bit(primes, p);
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

// decode [OPTIONS] [record.bits]
int main(int argc, char *argv[])
{
	message("%s: Sieve of Mersenne exponents, file decoder\n", argv[0]);

	int exponent_limit = -1;
	const char *record_path = "record.bits";
	const char *candidates_path = "candidates.txt";
	const char *factored_path = "factored.txt";

	// parse command-line options
	for(int opt; (opt = getopt(argc, argv, "h:")) != -1;)
	{
		switch(opt)
		{
			// -f FILE : exponents of factored M(p)
			case 'f':
				factored_path = optarg;
				break;
			// -p FILE : exponents of prime candidates
			case 'p':
				candidates_path = optarg;
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
		// decode [OPTIONS] [record.bits]
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

	// open candidates_path, factored_path
	FILE *candidates_file = fopen(candidates_path, "w");
	if( NULL == candidates_file )
	{
		message(ERR "Unable to open file '%s'.\n", candidates_path);
		exit(0);
	}
	FILE *factored_file = fopen(factored_path, "w");
	if( NULL == factored_file )
	{
		message(ERR "Unable to open file '%s'.\n", factored_path);
		exit(0);
	}

	for(int n = 1; n < exponent_limit; n++)
	{
		// for each prime
		if( is_prime(n, primes) )
		{
			// if marked as dirty
			if( get_bit(record, n) )
			{
				// write exponent into factored file
				if( fprintf(factored_file, "%i\n", n) < 0 )
				{
					message(ERR "Unable to write into file '%s'.\n", factored_path);
				}
			}
			else
			{
				// write it into candidates
				if( fprintf(candidates_file, "%i\n", n) < 0 )
				{
					message(ERR "Unable to write into file '%s'.\n", candidates_path);
				}
			}
		}
	}

	fclose(candidates_file);
	fclose(factored_file);

	free(record);
	free(primes);

	message("The program has finished.\n");

	return 0;
}
