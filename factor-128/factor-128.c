#include <libmp.h>
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>

#define INT128_H64L64(x) INT128_H64(x), INT128_L64(x)

static
int int128_clz(int128_t x)
{
	// high part
	if( x>>64 )
	{
		return __builtin_clzl((int64_t)(x>>64));
	}
	else
	// low part
	{
		return 64 + __builtin_clzl((int64_t)(x));
	}
}

int main(int argc, char *argv[])
{
	(void)argc;
	message("%s: Mersenne number factorization, 128-bit factors\n", argv[0]);

	// Find a factor of M(p), p : PRIME
	int128_t p = 74551753;

	message(INFO "factoring %" PRId64 ":%" PRId64 "... :)\n", INT128_H64L64(p));

	// check p : PRIME
	if( !mp_int128_is_prime_wheel30(p) )
	{
		message(ERR "%" PRId64 ":%" PRId64 " not a prime!\n", INT128_H64L64(p));
		return 1;
	}

	message(INFO "order-0 factoring...\n");

	// if ( p * 2 ) + 1 > MAX
	if( p > (INT128_MAX - 1) / 2 )
	{
		message(ERR "overflow!\n");
		return 1;
	}

	{
		int128_t q = ( p * 2 ) + 1;

		if( mp_int128_dpow2_pl_log(q, p) == 1 )
		{
			message(INFO "success, %" PRId64 ":%" PRId64 " | M(%" PRId64 ":%" PRId64 ")\n", INT128_H64L64(q), INT128_H64L64(p));
			return 0;
		}
	}

	int exponent_limit = 256*1024*1024; // 2^8 * 2^10 * 2^10
	int log2_exponent_limit = 8*(int)sizeof(int128_t) - int128_clz(exponent_limit-1); // 8 + 10 + 10
	int prefactored_bitlevel = 75;

	// create prime table
	uint8_t *primes = load_prime_table(exponent_limit);
	if( NULL == primes )
	{
		primes = gen_prime_table(exponent_limit);
		save_prime_table(primes, exponent_limit);
	}

	message(INFO "order-1 factoring...\n");

	if( p > (INT128_MAX - 1) / 2 )
	{
		message(ERR "p * 2 overflows!\n");
		return 1;
	}

	if( ( 8*(int)sizeof(int128_t) - int128_clz(p * 2) ) + log2_exponent_limit < prefactored_bitlevel )
	{
		message(WARN "skipping the order-1 factoring due to prefactored limit\n");
	}
	else
	{
		// for each prime 'a'
		for(int128_t a = 2; a; a = mp_int128_next_prime_cached(a, primes, exponent_limit))
		{
			// if ( p * a * 2 ) + 1 > MAX
			if( p > (INT128_MAX - 1) / 2 / a )
			{
				message(ERR "( p * a * 2 ) + 1 overflows!\n");
				return 1;
			}

			int128_t q = ( p * a * 2 ) + 1;

			if( mp_int128_dpow2_pl_log(q, p) == 1 )
			{
				message(INFO "success, %" PRId64 ":%" PRId64 " | M(%" PRId64 ":%" PRId64 ")\n", INT128_H64L64(q), INT128_H64L64(p));
				return 0;
			}
		}
	}

	message(INFO "order-2 factoring...\n");

	// for each prime 'a'
	for(int128_t a = 2; a; a = mp_int128_next_prime_cached(a, primes, exponent_limit))
	{
		message(DBG "progress: %" PRId64 ":%" PRId64 "/%" PRId64 ":%" PRId64 "\n", INT128_H64L64(a), INT128_H64L64((int128_t)exponent_limit));

		// q0 = p * a * 2
		if( a > (INT128_MAX - 1) / 2 / p )
		{
			message(ERR "p * a * 2 overflows!\n");
			return 1;
		}

		int128_t q0 = p * a * 2;

		if( ( 8*(int)sizeof(int128_t) - int128_clz(q0) ) + log2_exponent_limit < prefactored_bitlevel )
		{
			message(WARN "skipping internal loop in the order-2 factoring due to prefactored limit\n");
		}
		else
		{
			// for each prime 'b'
			for(int128_t b = 2; b; b = mp_int128_next_prime_cached(b, primes, exponent_limit))
			{
				// if ( p * a * b * 2 ) + 1 > MAX
				if( b > (INT128_MAX - 1) / q0 )
				{
					message(ERR "( p * a * b * 2 ) + 1 overflows!\n");
					return 1;
				}

				int128_t q = ( b * q0 ) + 1;

				if( mp_int128_dpow2_pl_log(q, p) == 1 )
				{
					message(INFO "success, %" PRId64 ":%" PRId64 " | M(%" PRId64 ":%" PRId64 ")\n", INT128_H64L64(q), INT128_H64L64(p));
					return 0;
				}
			}
		}
	}

	// ...
	message(INFO "no factor found\n");

	free(primes);

	message("The program has finished successfully.\n");

	return -1;
}
