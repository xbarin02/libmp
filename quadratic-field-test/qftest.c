#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <libmp.h>

// const int64_t D = -7;
// const int64_t D = -1;
const int64_t D = 2;
// const int64_t D = 3;

typedef struct { int64_t a, b; } qfield64_t;

// n^2 (mod M)
qfield64_t qfield64_square(qfield64_t n, int64_t M)
{
	return (qfield64_t){
		( mp_int64_dmul(M, n.a, n.a) + mp_int64_dmul(M, (M+D)%M, mp_int64_dmul(M, n.b, n.b)) ) % M,
		( 2*mp_int64_dmul(M, n.a, n.b) ) % M
	};
}

// n1*n2 (mod M)
qfield64_t qfield64_mul(qfield64_t n1, qfield64_t n2, int64_t M)
{
	return (qfield64_t){
		( mp_int64_dmul(M, n1.a, n2.a) + mp_int64_dmul(M, (M+D)%M, mp_int64_dmul(M, n1.b, n2.b)) ) %M,
		( mp_int64_dmul(M, n1.a, n2.b) + mp_int64_dmul(M, n2.a, n1.b) ) %M
	};
}

// ||n|| (mod M)
int64_t qfield64_norm(qfield64_t n, int64_t M)
{
	return ( mp_int64_dmul(M, n.a, n.a) + mp_int64_dmul(M, (M+D)%M, mp_int64_dmul(M, (M-n.b), n.b)) ) % M;
}

#define sym(x) ( ((x) > (M)/2) ? ((x)-(M)) : (x) )

void qfield64_print(qfield64_t n, int64_t M)
{
	printf("%li%+li√%li", sym(n.a), sym(n.b), D);
}

// b^k (mod M)
qfield64_t qfield64_pow(qfield64_t b, int k, int64_t M)
{
	assert( k >= 0 );

	qfield64_t m = (qfield64_t){ 1, 0 };

	while( k > 0 )
	{
		if( 1 & k )
		{
			m = qfield64_mul(m, b, M);
		}

		b = qfield64_square(b, M);

		k >>= 1;
	}

	return m;
}

int qftest(int64_t n)
{
	int64_t M = n;

// 	qfield64_t a = (qfield64_t){ 1, 1 };
// 	qfield64_t a = (qfield64_t){ 3, 2 };
// 	qfield64_t a = (qfield64_t){ 1, (M+1)/2 };
// 	qfield64_t a = (qfield64_t){ (M+1)/2, (M+1)/2 };
	qfield64_t a = (qfield64_t){ 1, 2 };
// 	qfield64_t a = (qfield64_t){ 2, 0 };

	qfield64_t s = a;

// 	printf("a^1 = "); qfield64_print(s, M); printf("; norm=%li", sym(qfield64_norm(s, M))); printf("\n");

	s = qfield64_pow(a, M, M);

// 	printf("a^M = "); qfield64_print(s, M); printf("; norm=%li", sym(qfield64_norm(s, M))); printf("\n");

	if( s.a==a.a && s.b==a.b )
	{
		return +1;
	}
	else if( s.a==a.a && M-s.b==a.b )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int main(int argc, char *argv[])
{
	message("%s: Fermat's little theorem in terms of quadratic fields\n", argv[0]);

	message("quadratic field: Q(√%li)\n", D);

	// for each number
	for(int64_t n = 3; n < 100000; n++)
	{
		if( !!mp_int64_is_prime(n) != !!qftest(n) )
		{
			if( mp_int64_is_prime(n) )
			{
				message("(reference) %li: PRIME\n", n);
			}
			else
			{
				message("(reference) %li: COMPOSITE\n", n);
			}

			if( qftest(n) )
			{
				message("(qftest) %li: PRIME\n", n);
			}
			else
			{
				message("(qftest) %li: COMPOSITE\n", n);
			}
		}
	}

	message("done\n");

	return 0;
}
