#ifndef FP_UTILITY_H__
#define FP_UTILITY_H__
#include <emp-tool/emp-tool.h>
using namespace emp;
using namespace std;

#define MERSENNE_PRIME_EXP 62
#define FIELD_TYPE uint64_t
const static __uint128_t p = 4611686018427322369;
const static int r = 1;
const static __uint128_t pr = 4611686018427322369;
const static block prs = makeBlock(4611686018427322369ULL, 4611686018427322369ULL);
const static uint64_t mask = 4611686018427387903ULL;
static __m128i masks = makeBlock(mask, mask);
const static uint64_t PR = 4611686018427322369;

const uint64_t add[8] = {
		0ULL,
		2305843009213693952ULL,
		65535ULL,
		2305843009213759487ULL,
		131070ULL,
		2305843009213825022ULL,
		196605ULL,
		2305843009213890557ULL,
};

static __m128i PRs = makeBlock(PR, PR);

#if  defined(__x86_64__) && defined(__BMI2__)
inline uint64_t mul64(uint64_t a, uint64_t b, uint64_t * c) {
	return _mulx_u64((unsigned long long )a, (unsigned long long) b, (unsigned long long*)c);
}
//
#else
inline uint64_t mul64(uint64_t a, uint64_t b, uint64_t * c) {
	__uint128_t aa = a;
	__uint128_t bb = b;
	auto cc = aa*bb;
	*c = cc>>64;
	return (uint64_t)cc;
}
#endif

inline uint64_t partial_mod(uint64_t x) {
	return (x >= p) ? x - p : x;
}

inline uint64_t mod(uint64_t x) {
	uint64_t tmp = x >> 62;
	uint64_t i = (x & mask) + (tmp << 16) - tmp;
	return (i >= p) ? i - p : i;
}

inline __uint128_t mod_uint128(__uint128_t k) {
	uint64_t hi = _mm_extract_epi64((block)k, 1);
	uint64_t lo = _mm_extract_epi64((block)k, 0);

	unsigned long long res;
	res = mod(lo);

	// add hi * 2^18
	unsigned long long res1;
	res1 = ((hi << 19) >> 19) << 18;
	res1 += (hi >> 45) << 17;
	res1 -= (hi >> 45) << 1;
	res1 = partial_mod(res1);

	// sub hi * 2^2
	unsigned long long res2;
	res2 = ((hi << 3) >> 3) << 2;
	res2 += (hi >> 61) << 17;
	res2 -= (hi >> 61) << 1;
	res2 = partial_mod(res2);
	res2 = p - res2;
	res2 = partial_mod(res2);

	res += res1;
	res = partial_mod(res);
	res += res2;
	res = partial_mod(res);

	return res;
}

inline block vec_partial_mod(block i) {
	return _mm_sub_epi64(i, _mm_andnot_si128(_mm_cmpgt_epi64(prs,i), prs));
}

inline block vec_mod(block i) {
	block tmp = _mm_srli_epi64(i, 62);
	block tmp1 = _mm_slli_epi64(tmp, 16);

	i = _mm_add_epi64((i & masks), tmp1);
	i = _mm_sub_epi64(i, tmp);

	return vec_partial_mod(i);
}

inline uint64_t mult_mod(uint64_t a, uint64_t b) {
	uint64_t lo, hi;
	lo = mul64(a, b, &hi);

	unsigned long long res;
	res = mod(lo);

	// add hi * 2^18
	unsigned long long res1;
	res1 = ((hi << 19) >> 19) << 18;
	res1 += (hi >> 45) << 17;
	res1 -= (hi >> 45) << 1;
	res1 = partial_mod(res1);

	// sub hi * 2^2
	unsigned long long res2;
	res2 = ((hi << 3) >> 3) << 2;
	res2 += (hi >> 61) << 17;
	res2 -= (hi >> 61) << 1;
	res2 = partial_mod(res2);
	res2 = p - res2;
	res2 = partial_mod(res2);

	res += res1;
	res = partial_mod(res);
	res += res2;
	res = partial_mod(res);

	return res;
}

inline block mult_mod(block a, uint64_t b) {
	uint64_t a_num[2];
	a_num[1] = _mm_extract_epi64(a, 1);
	a_num[0] = _mm_extract_epi64(a, 0);

	uint64_t c_num[2];
	for(int i = 0; i < 2; i++) {
		c_num[i] = mult_mod(a_num[i], b);
	}

	return makeBlock(c_num[1], c_num[0]);
}

inline block add_mod(block a, block b) {
	block res = _mm_add_epi64(a, b);
	return vec_partial_mod(res);
}

inline block add_mod(block a, uint64_t b) {
	block res = _mm_add_epi64(a, _mm_set_epi64((__m64)b, (__m64)b));
	return vec_partial_mod(res);
}

inline uint64_t add_mod(uint64_t a, uint64_t b) {
	uint64_t res = a + b;
	return (res >= PR) ? (res - PR) : res;
}

inline void extract_fp(__uint128_t& x) {
	x = mod(_mm_extract_epi64((block)x, 0));
}

template<typename T>
void uni_hash_coeff_gen(T* coeff, T seed, int sz) {
	coeff[0] = seed;
	for(int i = 1; i < sz; ++i)
		coeff[i] = mult_mod(coeff[i-1], seed);
}

template<typename T>
T vector_inn_prdt_sum_red(const T *a, const T *b, int sz) {
	T res = (T)0;
	for(int i = 0; i < sz; ++i)
		res = add_mod(res, mult_mod(a[i], b[i]));
	return res;
}

template<typename S, typename T>
T vector_inn_prdt_sum_red(const S *a, const T *b, int sz) {
	T res = (T)0;
	for(int i = 0; i < sz; ++i)
		res = add_mod(res, mult_mod((T)a[i], b[i]));
	return res;
}
#endif