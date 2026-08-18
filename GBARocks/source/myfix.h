#ifndef _MYFIX_H_
#define _MYFIX_H_
#include <stdlib.h>

typedef int32_t myfix;

extern const  myfix mySinTab[256];

#define MYFIX_INT_BITS 16
#define MYFIX_FRAC_BITS (32 - MYFIX_INT_BITS)

#define MYFIX_ONE ( 1 << MYFIX_FRAC_BITS)
#define MYFIX(value) ((myfix)((value) * MYFIX_ONE))

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

extern const myfix sinTab[256];
extern const myfix cosTab[256];
extern const myfix thrustX[256];
extern const myfix maxSpeedX[256];
extern const myfix thrustY[256];
extern const myfix maxSpeedY[256];
#endif // _MYFIX_H_
