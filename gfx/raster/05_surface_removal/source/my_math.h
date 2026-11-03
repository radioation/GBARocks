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
  float z;
  float w;
} Vec4;

typedef struct {
  float x;
  float y;
  float h; // 
} Point2;

typedef struct {
  uint16_t v0;
  uint16_t v1;
  uint16_t v2;
  uint16_t color;
} Triangle;


float dot3( Vec3 a, Vec3 b );
Vec3 add3( Vec3 a, Vec3 b );
Vec3 subtract3( Vec3 a,Vec3  b );
Vec3 mul3( Vec3 a, float v );
Vec3 cross3( Vec3 a, Vec3 b );

void mat4fXmat4f( float *a, float *b, float *retmat );
Vec4 mat4fXvec4f( float *a, Vec4 v );
void transposeMat4f( float* a, float *retmat );

Vec4 add4( Vec4 a, Vec4 b );
Vec4 subtract4( Vec4 a,Vec4  b );


#endif // _MY_MATH_H_
