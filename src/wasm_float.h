#ifndef REDIS_WASM_FLOAT_H
#define REDIS_WASM_FLOAT_H

static __inline unsigned __FLOAT_BITS_(float __f)
{
    union {float __f; unsigned __i;} __u;
    __u.__f = __f;
    return __u.__i;
}
static __inline unsigned long long __DOUBLE_BITS_(double __f)
{
    union {double __f; unsigned long long __i;} __u;
    __u.__f = __f;
    return __u.__i;
}

#undef fpclassify
#define fpclassify(x) ( \
	sizeof(x) == sizeof(float) ? __fpclassifyf(x) : __fpclassify(x))

#undef isinf
#define isinf(x) ( \
	sizeof(x) == sizeof(float) ? (__FLOAT_BITS_(x) & 0x7fffffff) == 0x7f800000 : (__DOUBLE_BITS_(x) & -1ULL>>1) == 0x7ffULL<<52)

#undef isnan
#define isnan(x) ( \
	sizeof(x) == sizeof(float) ? (__FLOAT_BITS_(x) & 0x7fffffff) > 0x7f800000 : (__DOUBLE_BITS_(x) & -1ULL>>1) > 0x7ffULL<<52)

#undef isnormal
#define isnormal(x) ( \
	sizeof(x) == sizeof(float) ? ((__FLOAT_BITS_(x)+0x00800000) & 0x7fffffff) >= 0x01000000 : ((__DOUBLE_BITS_(x)+(1ULL<<52)) & -1ULL>>1) >= 1ULL<<53)

#undef isfinite
#define isfinite(x) ( \
	sizeof(x) == sizeof(float) ? (__FLOAT_BITS_(x) & 0x7fffffff) < 0x7f800000 : (__DOUBLE_BITS_(x) & -1ULL>>1) < 0x7ffULL<<52)

#endif //REDIS_WASM_FLOAT_H
