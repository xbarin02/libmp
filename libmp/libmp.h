/** @file libmp.h
 * Mersenne prime library
 */
#ifndef LIBMP_H
#define LIBMP_H

#include <stdint.h>
#include <stddef.h>

#define ERR "ERROR: "
#define WARN "WARNING: "
#define DBG "DEBUG: "
#define INFO "INFO: "

int message(const char *format, ...);

uint8_t *load_prime_table(int exponent_limit);
uint8_t *gen_prime_table(int exponent_limit);
void save_prime_table(const uint8_t *primes, int exponent_limit);

/****************************************************************************/
/** @defgroup int int
 * @{
 */

int mp_int_ceil_sqrt(int n);

int mp_int_is_prime_cached(int p, const uint8_t *primes);
int mp_int_next_prime_cached(int p, const uint8_t *primes, int exponent_limit);

/** @} */
/****************************************************************************/

/****************************************************************************/
/** @defgroup int64_t int64_t
 * @{
 */

#define INT64_0 INT64_C(0)
#define INT64_1 INT64_C(1)
#define INT64_2 INT64_C(2)

int64_t mp_int64_dadd(int64_t p, int64_t a, int64_t b);

int64_t mp_int64_dmul(int64_t p, int64_t a, int64_t b);

int64_t mp_int64_dpow2_mn(int64_t p, int64_t K);
int64_t mp_int64_dpow2_mn_log(int64_t p, int64_t K);
int64_t mp_int64_dpow2_pl(int64_t p, int64_t K);
int64_t mp_int64_dpow2_pl_log(int64_t p, int64_t K);

int64_t mp_int64_dpow_pl_log(int64_t b, int64_t p, int64_t k);
int64_t mp_int64_pow_pl_log(int64_t b, int64_t k);

int64_t mp_int64_dlog2_mn(int64_t p);
int64_t mp_int64_dlog2_pl(int64_t p);
int64_t mp_int64_dlog2_bg(int64_t p);
int64_t mp_int64_element2_order(int64_t p);
int64_t mp_int64_element2_order_prtable(int64_t p, const uint8_t *primes, int exponent_limit);
int64_t mp_int64_element2_order_prtable2(int64_t p, const uint8_t *primes, int exponent_limit);
int64_t mp_int64_element2_order_prtable_exponents(int64_t p, const uint8_t *primes, int exponent_limit, const uint8_t *exponents, size_t P);

int64_t mp_int64_dlog2_mn_lim(int64_t p, int64_t L);
int64_t mp_int64_dlog2_pl_lim(int64_t p, int64_t L);
int64_t mp_int64_dlog2_bg_lim(int64_t p, int64_t L);

int64_t mp_int64_ceil_sqrt(int64_t n);
int64_t mp_int64_floor_sqrt(int64_t n);
int64_t mp_int64_ceil_div(int64_t a, int64_t b);

int mp_int64_is_prime(int64_t p);
int mp_int64_is_prime_wheel6(int64_t p);
int mp_int64_is_prime_wheel30(int64_t p);

int64_t mp_int64_next_prime_cached(int64_t p, const uint8_t *primes, int exponent_limit);

void mp_int64_test_prtest(uint8_t *record, int64_t factor, int exponent_limit, const uint8_t *primes);
void mp_int64_test_direct(uint8_t *record, int64_t factor, int exponent_limit, const uint8_t *primes);

int64_t mp_int64_inverse(int64_t a, int64_t n);
int64_t mp_int64_gcd(int64_t a, int64_t b);
int64_t mp_int64_divide(int64_t A, int64_t B, int64_t n);

int64_t mp_int64_floor_log2(int64_t n);

void mp_int64_factors_exponents(int64_t n, int64_t *factors, int64_t *exponents);

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

#define  INT128_C(h,l) ( ((( int128_t) INT64_C(h))<<64) | ( INT64_C(l)) )
#define UINT128_C(h,l) ( (((uint128_t)UINT64_C(h))<<64) | (UINT64_C(l)) )

#define  INT128_0  INT128_C(0,0)
#define  INT128_1  INT128_C(0,1)
#define  INT128_2  INT128_C(0,2)
#define UINT128_0 UINT128_C(0,0)
#define UINT128_1 UINT128_C(0,1)
#define UINT128_2 UINT128_C(0,2)

#define UINT128_L(x) ((x) & UINT64_MAX)
#define UINT128_H(x) ((x)>>64)

#define  INT128_L64(x) (( int64_t)((x)))
#define  INT128_H64(x) (( int64_t)((x)>>64))
#define UINT128_L64(x) ((uint64_t)((x)))
#define UINT128_H64(x) ((uint64_t)((x)>>64))

#define INT128_MAX (int128_t) (((uint128_t) 1 << ((__SIZEOF_INT128__ * __CHAR_BIT__) - 1)) - 1)
#define INT128_MIN (-INT128_MAX - 1)
#define UINT128_MIN ((int128_t) 0)
#define UINT128_MAX ( ~(uint128_t)0 )

int128_t mp_int128_dmul(int128_t p, int128_t a, int128_t b);

int128_t mp_int128_dpow2_mn(int128_t p, int128_t K);
int128_t mp_int128_dpow2_mn_log(int128_t p, int128_t K);
int128_t mp_int128_dpow2_pl(int128_t p, int128_t K);
int128_t mp_int128_dpow2_pl_log(int128_t p, int128_t K);

int128_t mp_int128_dpow_pl_log(int128_t b, int128_t p, int128_t k);

int128_t mp_int128_dlog2_mn(int128_t p);
int128_t mp_int128_dlog2_pl(int128_t p);
int128_t mp_int128_dlog2_bg(int128_t p);
int128_t mp_int128_element2_order(int128_t p);

int128_t mp_int128_dlog2_mn_lim(int128_t p, int128_t L);
int128_t mp_int128_dlog2_pl_lim(int128_t p, int128_t L);
int128_t mp_int128_dlog2_bg_lim(int128_t p, int128_t L);

int128_t mp_int128_ceil_sqrt(int128_t n);
int128_t mp_int128_floor_sqrt(int128_t n);
int128_t mp_int128_ceil_div(int128_t a, int128_t b);

int mp_int128_is_prime(int128_t p);
int mp_int128_is_prime_wheel6(int128_t p);
int mp_int128_is_prime_wheel30(int128_t p);

int128_t mp_int128_next_prime_cached(int128_t p, const uint8_t *primes, int exponent_limit);

void mp_int128_test_prtest(uint8_t *record, int128_t factor, int exponent_limit, const uint8_t *primes);
void mp_int128_test_direct(uint8_t *record, int128_t factor, int exponent_limit, const uint8_t *primes);

int128_t mp_int128_inverse(int128_t a, int128_t n);
int128_t mp_int128_gcd(int128_t a, int128_t b);
int128_t mp_int128_divide(int128_t A, int128_t B, int128_t n);

int128_t mp_int128_floor_log2(int128_t n);

void mp_int128_factors_exponents(int128_t n, int128_t *factors, int128_t *exponents);

/** @} */
/****************************************************************************/

#endif
