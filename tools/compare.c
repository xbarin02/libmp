#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include <libmp.h>

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

// compare [OPTIONS] [record.bits] [record.bits]
int main(int argc, char *argv[])
{
	message("%s: Sieve of Mersenne exponents, record comparator\n", argv[0]);

	int exponent_limit = -1;
	const char *record0_path = "record.bits";
	const char *record1_path = "record.bits";
	int verbose = 0;

	// parse command-line options
	for(int opt; (opt = getopt(argc, argv, "h:v")) != -1;)
	{
		switch(opt)
		{
			// -h EXP : highest exponent in the record
			case 'h':
				exponent_limit = atoi(optarg);
				if( exponent_limit < 2 )
				{
					message(WARN "Invalid exponent, keeping the default one!\n");
					exponent_limit = -1;
				}
				break;
			// -v : verbose (print the differences)
			case 'v':
				verbose = 1;
				break;
			default :
				message(WARN "Unknown option :( Read the source code!\n");
		}
	}

	// arguments after options
	if(optind < argc)
	{
		// compare [OPTIONS] [record.bits]
		record0_path = argv[optind++];
	}

	if(optind < argc)
	{
		// compare [OPTIONS] [record.bits] [record.bits]
		record1_path = argv[optind++];
	}

	if(optind < argc)
	{
		message(WARN "Too much options :( Read the source code!\n");
	}

	// open the records
	char *record0 = record_load(&exponent_limit, record0_path);
	char *record1 = record_load(&exponent_limit, record1_path);

	// create prime table
	char *primes = (char *)load_prime_table(exponent_limit);
	if( NULL == primes )
	{
		primes = (char *)gen_prime_table(exponent_limit);
		save_prime_table((uint8_t *)primes, exponent_limit);
	}

	int less = 1, equal = 1, greater = 1;
	int rec0_cnt = 0, rec1_cnt = 0;

	for(int n = 1; n < exponent_limit; n++)
	{
		// for each prime
		if( is_prime(n, primes) )
		{
			int b0 = get_bit(record0, n);
			int b1 = get_bit(record1, n);

			if( b0 < b1 ) // 0-1
			{
				equal = 0;
				greater = 0;
				rec1_cnt++;

				if(verbose)
					message("<\tM(%i)\n", n);
			}
			else if( b0 > b1 ) // 1-0
			{
				equal = 0;
				less = 0;
				rec0_cnt++;

				if(verbose)
					message(">\tM(%i)\n", n);
			}
			else // 0-0 or 1-1
			{}
		}
	}

	if( equal )
		message("'%s' = '%s'\n", record0_path, record1_path); // is equal to
	else if( less )
		message("'%s' < '%s'\n", record0_path, record1_path); // is subset of
	else if( greater )
		message("'%s' > '%s'\n", record0_path, record1_path); // is superset of
	else
		message("'%s' (+%i) != '%s' (+%i) (the records are completely different)\n", record0_path, rec0_cnt, record1_path, rec1_cnt);

	free(record0);
	free(record1);
	free(primes);

	message("The program has finished.\n");

	return 0;
}
