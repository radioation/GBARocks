#include "my_math.h"
#include <math.h>

float dot( Vec3 a, Vec3 b ){
        return a.x * b.x + a.y* b.y + a.z  * b.z ;
}
Vec3 add( Vec3 a, Vec3 b ) {
 return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}
Vec3 subtract( Vec3 a,Vec3  b ) {
 return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

float length( Vec3 a ) {
  return sqrt( a.x * a.x +a.y*a.y + a.z * a.z );
}

Vec3 mul( Vec3 a, float scale ) {
  return (Vec3) { a.x * scale, a.y * scale, a.z * scale };
}

Vec3 divide( Vec3 a, float val ) {
  return (Vec3) { a.x /val, a.y / val, a.z  / val };
}

/*
float dot3f( float *a, float *b ){
        return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void cross3f( float *a, float* b, float *retvect ) {
        retvect[0] = a[1]*b[2] - a[2]*b[1];
        retvect[1] = a[2]*b[0] - a[0]*b[2];
        retvect[2] = a[0]*b[1] - a[1]*b[0];

}
*/

