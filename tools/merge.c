#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <libmp.h>

// merge records
void record_merge(char *output_record, const char *input_record, int exponent_limit)
{
	size_t size = (size_t)(exponent_limit+7)/8;

	for(size_t i = 0; i < size; i++)
	{
		output_record[i] |= input_record[i];
	}
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

// merge [-o output] [input1] [... inputN]
int main(int argc, char *argv[])
{
	message("%s: Sieve of Mersenne exponents, record merger\n", argv[0]);

	int exponent_limit = -1;
	const char *output_path = "merged.bits";
	int reset = 0;

	// parse command-line options
	for(int opt; (opt = getopt(argc, argv, "o:rh:")) != -1;)
	{
		switch(opt)
		{
			// -o FILE : output (merged) file
			case 'o':
				output_path = optarg;
				break;
			// -r : reset the output record
			case 'r':
				reset = 1;
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

	// open output record
	char *output_record = record_load(&exponent_limit, output_path);

	if(reset)
	{
		bzero(output_record, (size_t)(exponent_limit+7)/8);
		message("Resetting the record... Created empty record of %i exponents in size (%i MiB in memory, %i MiB in file)!\n",
			(exponent_limit),
			(exponent_limit + (1<<23) - 1)>>23,
			(exponent_limit + (1<<23) - 1)>>23
		);
	}

	// for each input record
	while(optind < argc)
	{
		const char *input_path = argv[optind++];

		char *input_record = record_load(&exponent_limit, input_path);

		record_merge(output_record, input_record, exponent_limit);

		message("'%s' has been merged.\n", input_path);

		free(input_record);
	}

	// save the record
	record_save(output_record, exponent_limit, output_path);

	free(output_record);

	message("The program has finished.\n");

	return 0;
}
