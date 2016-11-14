#ifndef LIBMP_H
#define LIBMP_H

/****************************************************************************/
/** @defgroup int64_t int64_t
 * @{
 */

#include <stdint.h>

#define INT64_0 INT64_C(0)
#define INT64_1 INT64_C(1)
#define INT64_2 INT64_C(2)

int64_t mp_int64_dpow2_pl(int64_t p, int64_t K);
int64_t mp_int64_dpow2_mn(int64_t p, int64_t K);

int64_t mp_int64_dlog2_mn(int64_t p);
int64_t mp_int64_dlog2_pl(int64_t p);

int64_t mp_int64_dlog2_mn_lim(int64_t p, int64_t L);

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

int128_t mp_int128_dpow2_pl(int128_t p, int128_t K);
int128_t mp_int128_dpow2_mn(int128_t p, int128_t K);

int128_t mp_int128_dlog2_mn(int128_t p);
int128_t mp_int128_dlog2_pl(int128_t p);

/** @} */
/****************************************************************************/

#endif
