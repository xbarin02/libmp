/** @file libmp.h
 * Mersenne prime library
 */
#ifndef LIBMP_H
#define LIBMP_H

#include <stdint.h>

#define ERR "ERROR: "
#define WARN "WARNING: "
#define DBG "DEBUG: "

int message(const char *format, ...);

/****************************************************************************/
/** @defgroup int int
 * @{
 */

int mp_int_is_prime_cached(int p, const uint8_t *primes);

/** @} */
/****************************************************************************/

/****************************************************************************/
/** @defgroup int64_t int64_t
 * @{
 */

#define INT64_0 INT64_C(0)
#define INT64_1 INT64_C(1)
#define INT64_2 INT64_C(2)

int64_t mp_int64_dpow2_mn(int64_t p, int64_t K);
int64_t mp_int64_dpow2_mn_log(int64_t p, int64_t K);
int64_t mp_int64_dpow2_pl(int64_t p, int64_t K);
int64_t mp_int64_dpow2_pl_log(int64_t p, int64_t K);

int64_t mp_int64_dlog2_mn(int64_t p);
int64_t mp_int64_dlog2_pl(int64_t p);
int64_t mp_int64_dlog2_bg(int64_t p);

int64_t mp_int64_dlog2_mn_lim(int64_t p, int64_t L);
int64_t mp_int64_dlog2_pl_lim(int64_t p, int64_t L);
int64_t mp_int64_dlog2_bg_lim(int64_t p, int64_t L);

int64_t mp_int64_ceil_sqrt(int64_t n);
int64_t mp_int64_ceil_div(int64_t a, int64_t b);

int mp_int64_is_prime(int64_t p);

void mp_int64_test_prtest(uint8_t *record, int64_t factor, int exponent_limit, const uint8_t *primes);
void mp_int64_test_direct(uint8_t *record, int64_t factor, int exponent_limit, const uint8_t *primes);

int64_t mp_int64_inverse(int64_t a, int64_t n);
int64_t mp_int64_gcd(int64_t a, int64_t b);

/** @} */
/****************************************************************************/

/****************************************************************************/
/** @defgroup int128_t int128_t
 * @{
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-pedantic"
typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;
#pragma GCC diagnostic pop

#define INT128_C(c) ((int128_t)INT64_C(c))
#define INT128_0 INT128_C(0)
#define INT128_1 INT128_C(1)
#define INT128_2 INT128_C(2)
#define INT128_L64(x) ((int64_t)(x))
#define INT128_H64(x) ((int64_t)((x)>>64))
#define INT128_MAX (int128_t) (((uint128_t) 1 << ((__SIZEOF_INT128__ * __CHAR_BIT__) - 1)) - 1)
#define INT128_MIN (-INT128_MAX - 1)

int128_t mp_int128_dpow2_mn(int128_t p, int128_t K);
int128_t mp_int128_dpow2_mn_log(int128_t p, int128_t K);
int128_t mp_int128_dpow2_pl(int128_t p, int128_t K);
int128_t mp_int128_dpow2_pl_log(int128_t p, int128_t K);

int128_t mp_int128_dlog2_mn(int128_t p);
int128_t mp_int128_dlog2_pl(int128_t p);
int128_t mp_int128_dlog2_bg(int128_t p);

int128_t mp_int128_dlog2_mn_lim(int128_t p, int128_t L);
int128_t mp_int128_dlog2_pl_lim(int128_t p, int128_t L);
int128_t mp_int128_dlog2_bg_lim(int128_t p, int128_t L);

int128_t mp_int128_ceil_sqrt(int128_t n);
int128_t mp_int128_ceil_div(int128_t a, int128_t b);

int mp_int128_is_prime(int128_t p);

void mp_int128_test_prtest(uint8_t *record, int128_t factor, int exponent_limit, const uint8_t *primes);
void mp_int128_test_direct(uint8_t *record, int128_t factor, int exponent_limit, const uint8_t *primes);

int128_t mp_int128_inverse(int128_t a, int128_t n);
int128_t mp_int128_gcd(int128_t a, int128_t b);

/** @} */
/****************************************************************************/

#endif
