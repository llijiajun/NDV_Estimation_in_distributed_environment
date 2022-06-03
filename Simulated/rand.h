#ifndef RAND
#define RAND

#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <omp.h>

using namespace std;

#ifndef UINT64_C
#define UINT64_C(c) (c##ULL)
#endif

typedef unsigned long long ull;

uint64_t rng_seed[2];

bool SEED_SET(0);

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

uint64_t lrand() {
	const uint64_t s0 = rng_seed[0];
	uint64_t s1 = rng_seed[1];
	const uint64_t result = s0 + s1;
	s1 ^= s0;
	rng_seed[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
	rng_seed[1] = rotl(s1, 36);                   // c
	return result;
}

void SET_SEED(int seed){
	ull x = (ull)seed;
	for (int i = 0; i < 2; i++) {
		ull z = x += UINT64_C(0x9E3779B97F4A7C15);
		z = (z ^ z >> 30) * UINT64_C(0xBF58476D1CE4E5B9);
		z = (z ^ z >> 27) * UINT64_C(0x94D049BB133111EB);
		rng_seed[i] = z ^ z >> 31;
	}
	SEED_SET=1;
}
void SET_SEED(){
	ull x = time(nullptr);
	for (int i = 0; i < 2; i++) {
		ull z = x += UINT64_C(0x9E3779B97F4A7C15);
		z = (z ^ z >> 30) * UINT64_C(0xBF58476D1CE4E5B9);
		z = (z ^ z >> 27) * UINT64_C(0x94D049BB133111EB);
		rng_seed[i] = z ^ z >> 31;
	}
	SEED_SET=1;
}

static inline double drand() {
	if(!SEED_SET){
		SET_SEED();
	}
	const union un {
		uint64_t i;
		double d;
	} a = {UINT64_C(0x3FF) << 52 | lrand() >> 12};
	return a.d - 1.0;
}


inline int irand(int min, int max) { return lrand() % (max - min) + min; }

inline int irand(int max) { return lrand() % max; }

static inline void rnorm(double& x,double mean,double st){
	const double PI(3.1415926535897932384626433832795028841971693993751);
	double v1=drand();
	double v2=drand();
	double len=sqrt(-2.0*log(v1));
	x=st*(len*sin(2.0*PI*v2))+mean;
}

#endif

