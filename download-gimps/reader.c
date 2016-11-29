#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <libmp.h>

static
void set_bit(char *ptr, int i)
{
	ptr[i/8] |= 1 << i%8;
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
		int errsv = errno;
		message(ERR "Unable to save the record :( Error: %s\n", strerror(errsv));
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

enum state_t {
	ST_START, // at the beginning, expecting "Exponent" => ST_START_EXPONENT
	ST_START_EXPONENT, // after "Exponent", expecting "Status" => ST_START_STATUS
	ST_START_STATUS, // after "Status", expecting "Data" => ST_READY
	ST_READY, // expecting "%li" exponent, => ST_EXPONENT
	ST_EXPONENT, // got exponent, expecting eighter "Prime" (=> ST_READY) or "Factored" (=> ST_FACTORED) or "Unfactored" (=> ST_UNFACTORED) or "PM1" (=> ST_PM1) or "LL" (=> ST_LL) or "Exponent" (=> ST_START_EXPONENT)
	ST_FACTORED,
	ST_UNFACTORED,
	ST_PM1,
	ST_LL,
	ST_ERROR,
	ST_EOF
};

// reader [OPTIONS] < stdin
int main(int argc, char *argv[])
{
	message("%s: Sieve of Mersenne exponents, mersenne.org parser\n", argv[0]);

	int exponent_limit = -1;
	const char *factored_path = "factored.bits";
	const char *lltested_path = "lltested.bits";
	FILE *input = stdin;

	// parse command-line options
	for(int opt; (opt = getopt(argc, argv, "h:")) != -1;)
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
			default :
				message(WARN "Unknown option :( Read the source code!\n");
		}
	}

	// argument after options
	if(optind < argc)
	{
		message(WARN "Too much options :( Read the source code!\n");
	}

	// load the records
	char *factored_record = record_load(&exponent_limit, factored_path);
	char *lltested_record = record_load(&exponent_limit, lltested_path);

	enum state_t state = ST_START;
	int run = 1;

	while(run)
	{
		char buff[4096];
		int ret;
		int64_t exp;

		switch(state)
		{
#			define NEXT_STATE(next_state) { /*printf("\tNEXT: %s\n", #next_state);*/ state = (next_state); break; }
			// ----------------------------------------------------
			case ST_START:
				ret = fscanf(input, "%s", buff);
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 0 == ret )
					NEXT_STATE(ST_ERROR);
				if( 0 == strcmp(buff, "Exponent") )
					NEXT_STATE(ST_START_EXPONENT);
				NEXT_STATE(ST_ERROR);
			// ----------------------------------------------------
			case ST_START_EXPONENT:
				ret = fscanf(input, "%s", buff);
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 0 == ret )
					NEXT_STATE(ST_ERROR);
				if( 0 == strcmp(buff, "Status") )
					NEXT_STATE(ST_START_STATUS);
				NEXT_STATE(ST_ERROR);
			// ----------------------------------------------------
			case ST_START_STATUS:
				ret = fscanf(input, "%s", buff);
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 0 == ret )
					NEXT_STATE(ST_ERROR);
				if( 0 == strcmp(buff, "Data") )
					NEXT_STATE(ST_READY);
				NEXT_STATE(ST_ERROR);
			// ----------------------------------------------------
			case ST_READY:
				ret = fscanf(input, "%" SCNd64, &exp);
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 0 == ret )
					NEXT_STATE(ST_ERROR);
				printf("EXPONENT: %" PRId64 "\n", exp);
				NEXT_STATE(ST_EXPONENT);
			// ----------------------------------------------------
			case ST_EXPONENT:
				ret = fscanf(input, "%s", buff);
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 0 == ret )
					NEXT_STATE(ST_ERROR);
				if( 0 == strcmp(buff, "Prime") )
				{
					printf("PRIME\n");
					NEXT_STATE(ST_READY);
				}
				if( 0 == strcmp(buff, "Factored") )
				{
					printf("FACTORED\n");
					NEXT_STATE(ST_FACTORED);
				}
				if( 0 == strcmp(buff, "Unfactored") )
				{
					printf("UNFACTORED\n");
					NEXT_STATE(ST_UNFACTORED);
				}
				if( 0 == strcmp(buff, "PM1") )
				{
					printf("PM1\n");
					NEXT_STATE(ST_PM1);
				}
				if( 0 == strcmp(buff, "LL") )
				{
					printf("LL\n");
					NEXT_STATE(ST_LL);
				}
				if( 0 == strcmp(buff, "Exponent") )
				{
					printf("RESTART\n");
					NEXT_STATE(ST_START_EXPONENT);
				}
				printf("UNHANDLED %s\n", buff);
				NEXT_STATE(ST_ERROR);
			// ----------------------------------------------------
			case ST_FACTORED:
				ret = fscanf(input, "%s", buff);
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 0 == ret )
					NEXT_STATE(ST_ERROR);
				// TODO: mark as composite in FACTOR table
				set_bit(factored_record, (int)exp);
				NEXT_STATE(ST_READY);
			// ----------------------------------------------------
			case ST_LL:
				ret = fscanf(input, " %[^;]", buff); // read "Verified", "Verified (Factored)", "Unverified"
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 1 != ret )
					NEXT_STATE(ST_ERROR);
				printf("\t\tSTATE: '%s'\n", buff);
				*buff = 0;
				ret = fscanf(input, ";%[^;]", buff); // read date "2011-10-03" or fails due to empty string
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 1 == ret )
					printf("\t\tDATE: '%s'\n", buff);
				else
					printf("\t\tDATE: (empty)\n");
				*buff = 0;
				ret = fscanf(input, ";%[^;]", buff); // read name or fails due to empty string
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 1 == ret )
					printf("\t\tNAME: '%s'\n", buff);
				else
					printf("\t\tNAME: (empty)\n");
				ret = fscanf(input, ";%s", buff); // read residue "12ACD05A4339E0__"
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 1 != ret )
					NEXT_STATE(ST_ERROR);
				printf("\t\tRESIDUE: '%s'\n", buff);
				// TODO: mark as composite in LL table
				set_bit(lltested_record, (int)exp);
				NEXT_STATE(ST_READY);
			// ----------------------------------------------------
			case ST_UNFACTORED:
				ret = fscanf(input, " %s", buff); // read "2^63"
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 1 != ret )
					NEXT_STATE(ST_ERROR);
				NEXT_STATE(ST_READY);
			// ----------------------------------------------------
			case ST_PM1:
				ret = fscanf(input, " %s", buff); // read "B1=4356913107423,B2=348553048593840"
				if( EOF == ret )
					NEXT_STATE(ST_EOF);
				if( 1 != ret )
					NEXT_STATE(ST_ERROR);
				NEXT_STATE(ST_READY);
			// ----------------------------------------------------
			case ST_EOF:
				printf("EOF\n");
				run = 0;
				NEXT_STATE(ST_ERROR);
			// ----------------------------------------------------
			case ST_ERROR:
				printf("ERROR\n");
				run = 0;
				NEXT_STATE(ST_ERROR);
			// ----------------------------------------------------
			default:
				assert( !"implemented" );
#			undef NEXT_STATE
		}
	}

	printf("DONE\n");

	// save the records
	record_save(lltested_record, exponent_limit, lltested_path);
	record_save(factored_record, exponent_limit, factored_path);

	free(lltested_record);
	free(factored_record);

	message("The program has finished successfully.\n");

	return 0;
}
