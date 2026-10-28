#ifndef _MY_MATH_H_
#define _MY_MATH_H_
#include <stdint.h>


typedef struct {
    float x;
     float y;
     float z;
} Vec3;

typedef struct {
    float x;
     float y;
} Point2;



float dot( Vec3 a, Vec3 b );
Vec3 add( Vec3 a, Vec3 b );
Vec3 subtract( Vec3 a,Vec3  b );
float length( Vec3 a );
Vec3 mul( Vec3 a, float scale );
Vec3 divide( Vec3 a, float val );
//float cross( float *a, float *b );

#endif // _MY_MATH_H_
