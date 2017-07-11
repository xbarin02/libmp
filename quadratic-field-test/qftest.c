#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <libmp.h>

// test a^(p-1) == 1 rather than a^p == p
#define TEST_P1

// a + b*sqrt(D)
typedef struct { int64_t a, b; } qfield64_t;

// n^2 (mod M)
qfield64_t qfield64_square(qfield64_t n, int64_t M, int64_t D)
{
	return (qfield64_t){
		mp_int64_dadd(M, mp_int64_dmul(M, n.a, n.a), mp_int64_dmul(M, mp_int64_dadd(M, M, D), mp_int64_dmul(M, n.b, n.b))),
		mp_int64_dadd(M, mp_int64_dmul(M, n.a, n.b), mp_int64_dmul(M, n.a, n.b))
	};
}

// n1*n2 (mod M)
qfield64_t qfield64_mul(qfield64_t n1, qfield64_t n2, int64_t M, int64_t D)
{
	return (qfield64_t){
		mp_int64_dadd(M, mp_int64_dmul(M, n1.a, n2.a), mp_int64_dmul(M, mp_int64_dadd(M, M, D), mp_int64_dmul(M, n1.b, n2.b))),
		mp_int64_dadd(M, mp_int64_dmul(M, n1.a, n2.b), mp_int64_dmul(M, n2.a, n1.b))
	};
}

// ||n|| (mod M)
int64_t qfield64_norm(qfield64_t n, int64_t M, int64_t D)
{
	return ( mp_int64_dmul(M, n.a, n.a) + mp_int64_dmul(M, (M+D)%M, mp_int64_dmul(M, (M-n.b), n.b)) ) % M;
}

void qfield64_print(qfield64_t n, int64_t M, int64_t D)
{
#	define sym(x) ( ((x) > (M)/2) ? ((x)-(M)) : (x) )
	printf("%li%+li√%li (norm=%li)", sym(n.a), sym(n.b), D, sym(qfield64_norm(n, M, D)));
#	undef sym
}

// b^k (mod M)
qfield64_t qfield64_pow(qfield64_t b, int k, int64_t M, int64_t D)
{
	assert( k >= 0 );

	qfield64_t m = (qfield64_t){ 1, 0 };

	while( k > 0 )
	{
		if( 1 & k )
		{
			m = qfield64_mul(m, b, M, D);
		}

		b = qfield64_square(b, M, D);

		k >>= 1;
	}

	return m;
}

int qftest(int64_t n, int64_t D, qfield64_t a)
{
	int64_t M = n;

	assert( a.a >= 0 && a.b >= 0 && "the base should be in (mod M)" );

	a.a %= M;
	a.b %= M;

	assert( a.a < M && a.b < M && "the base should be in (mod M)"  );

	if( a.a == 0 )
	{
// 		message("REJECTED %li, may be a prime\n", M);
		return 1;
	}

	qfield64_t s = a;

// 	printf("a^1 = "); qfield64_print(s, M, D); printf("\n");

#ifndef TEST_P1
	s = qfield64_pow(a, M, M, D); // a^p
#else
	s = qfield64_pow(a, M-1, M, D); // a^(p-1)
#endif

// 	printf("a^M = "); qfield64_print(s, M, D); printf("\n");

#ifndef TEST_P1
	if( s.a==a.a && s.b==a.b )
	{
		return +1;
	}
	else if( s.a==a.a && M-s.b==a.b )
	{
		return -1;
	}
#else
	// e.g, testing number M=66 with base a=3
	if( a.b == 0 && 0 == M % a.a )
	{
// 		message(WARN "COMPOSITE by a base (%li) cannot be divisible by 'p' (%li)\n", a.a, M);
		return 0;
	}

	// numerator / denominator
	int64_t antiunit_denominator = qfield64_norm(a, M, D);
	qfield64_t antiunit_numerator = qfield64_square((qfield64_t){ a.a, M-a.b }, M, D);
	qfield64_t s_mul_antiunit_denominator = (qfield64_t){ (s.a*antiunit_denominator)%M, (s.b*antiunit_denominator)%M };

	if( s.a==1 && s.b==0 )
	{
// 		message("(+): s = "); qfield64_print(s, M, D); printf("\n");
		return +1;
	}
	else if( antiunit_denominator > 0 && ( s_mul_antiunit_denominator.a == antiunit_numerator.a && s_mul_antiunit_denominator.b == antiunit_numerator.b ) )
	{
// 		message("(-): s = "); qfield64_print(s, M, D); printf("\n");
		return -1;
	}
#endif

	return 0;
}

int qftest2(int64_t n)
{
	int result = 1;

// 	result *= qftest(n, /*D*/+1, (qfield64_t){2, 0} ); // 2
// 	result *= qftest(n, /*D*/+1, (qfield64_t){3, 0} ); // 3
// 	result *= qftest(n, /*D*/+1, (qfield64_t){5, 0} ); // 5
// 	result *= qftest(n, /*D*/+1, (qfield64_t){7, 0} ); // 7

// 	result *= qftest(n, /*D*/-2, (qfield64_t){1, 1} ); // 1+i*sqrt(2)
// 	result *= qftest(n, /*D*/-1, (qfield64_t){1, 1} ); // 1+i
// 	result *= qftest(n, /*D*/+2, (qfield64_t){1, 1} ); // 1+sqrt(2)
// 	result *= qftest(n, /*D*/+3, (qfield64_t){1, 1} ); // 1+sqrt(3)
	result *= qftest(n, /*D*/+7, (qfield64_t){1, 1} ); // 1+sqrt(7)

// 	result *= qftest(n, /*D*/+2, (qfield64_t){3, 2} ); // 3+2sqrt(2)
// 	result *= qftest(n, /*D*/+2, (qfield64_t){ 1, (n+1)/2 } ); // 1 + ...
// 	result *= qftest(n, /*D*/+2, (qfield64_t){ (n+1)/2, (n+1)/2 } ); // ... + ...

	return result;
}

int main(int argc, char *argv[])
{
	message("%s: Fermat's little theorem in terms of quadratic fields\n", argv[0]);

	int64_t pseudoprimes = 0;
	int64_t pseudoprimes_p = 0;
	int64_t pseudoprimes_n = 0;
	int64_t failed = 0;

// 	int64_t bound = 20;
// 	int64_t bound = 1000;
	int64_t bound = 100000;
// 	int64_t bound = 1000000;

	// print differences
	int debug = 1;

	// print list of pseudoprimes
	int print_pseudoprimes = 1;

	if( print_pseudoprimes )
		printf("pseudoprimes: ");

	// for each number
	for(int64_t n = 3; n < bound; n++)
	{
		int r = mp_int64_is_prime(n);
		int t = qftest2(n);

		if( !!r != !!t )
		{
			if( !!t ) pseudoprimes++;
			if( t > 0 ) pseudoprimes_p++;
			if( t < 0 ) pseudoprimes_n++;
			if( !!r ) failed++;
			if( !!r && debug ) printf("FAIL: %li is PRIME, result was COMPOSITE\n", n);
			if( !!t && print_pseudoprimes ) printf("%li, ", n);
		}
	}

	if( print_pseudoprimes )
		printf(", ...\n");

	message("done: %li pseudoprimes (+: %li, -: %li), %li fails under %li\n", pseudoprimes, pseudoprimes_p, pseudoprimes_n, failed, bound);

	return 0;
}
