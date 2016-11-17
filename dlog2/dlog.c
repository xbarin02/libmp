#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <libmp.h>

int main(int argc, char *argv[])
{
	switch(argc)
	{
		case 1:
			printf("Usage:\n\t%s [f]\t.. returns dlog2(1) (mod f)\n\t%s [f] [r]\t.. returns 2^r (mod f)\n", *argv, *argv);
			break;
		case 2:
		{
			int64_t f = atol(argv[1]);
			printf("dlog2(%" PRId64 ") = %" PRId64 " (MSB)\n", f, mp_int64_dlog2_pl(f));
			printf("dlog2(%" PRId64 ") = %" PRId64 " (LSB)\n", f, mp_int64_dlog2_mn(f));
			printf("dlog2(%" PRId64 ") = %" PRId64 " (BGA)\n", f, mp_int64_dlog2_bg(f));
		}
			break;
		case 3:
		{
			int64_t f = atol(argv[1]);
			int64_t r = atol(argv[2]);
			printf("(mod %" PRId64 ") 2^(+%" PRId64 ") = %" PRId64 " (MSB)\n", f, r, mp_int64_dpow2_pl(f, r));
			printf("(mod %" PRId64 ") 2^(-%" PRId64 ") = %" PRId64 " (LSB)\n", f, r, mp_int64_dpow2_mn(f, r));
		}
			break;
		default:
			return main(1, argv);
	}

	return 0;
}
