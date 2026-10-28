#include "my_math.h"

float dot( Vec3 a, Vec3 b ){
        return a.x * b.x + a.y* b.y + a.z  * b.z ;
}
Vec3 add( Vec3 a, Vec3 b ) {
 return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}
Vec3 subtract( Vec3 a,Vec3  b ) {
 return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
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

