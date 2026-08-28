#ifndef _MYFIX_H_
#define _MYFIX_H_

typedef int32_t myfix;

extern const  myfix mySinTab[256];

#define MYFIX_INT_BITS 16
#define MYFIX_FRAC_BITS (16 - MYFIX_INT_BITS)

#define FIX_ONE ( 1 << MYFIX_FRAC_BITS)
#define FIX(value) ((myfix)((value) * FIX_ONE))

static inline myfix intToFix( int32_t x ) {
	return (x << MYFIX_FRAC_BITS);
}
static inline myfix fixToInt( int32_t x ) {
	return x >> MYFIX_FRAC_BITS;
}

static inline myfix fix_mul(myfix v1, myfix v2)
{
    return (myfix)(((int64_t)v1 * v2) >> MYFIX_FRAC_BITS);
}

static inline myfix fix_div(myfix v1, myfix v2)
{
    return (myfix)(((int64_t)v1 << MYFIX_FRAC_BITS) / v2);
}

#endif // _MYFIX_H_
