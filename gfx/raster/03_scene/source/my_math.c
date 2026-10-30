#include "my_math.h"

float dot3( Vec3 a, Vec3 b ){
  return a.x * b.x + a.y* b.y + a.z  * b.z ;
}
Vec3 add3( Vec3 a, Vec3 b ) {
  return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}
Vec3 subtract3( Vec3 a,Vec3  b ) {
  return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 cross3( Vec3 a, Vec3 b ) {
  Vec3 retvec;
  retvec.x = a.y*b.z - a.z*b.y;
  retvec.y = a.z*b.x - a.x*b.z;
  retvec.z = a.x*b.y - a.y*b.x;
	return retvec;
}

//void cross3f( float *a, float* b, float *retvect ) {
//  retvect[0] = a[1]*b[2] - a[2]*b[1];
//  retvect[1] = a[2]*b[0] - a[0]*b[2];
//  retvect[2] = a[0]*b[1] - a[1]*b[0];
//
//}


void mat4fXmat4f( float *a, float *b, float *retmat ){
  retmat[0]  = a[0]*b[0] + a[1]*b[4] + a[2]*b[8] + a[3]*b[12];
  retmat[1]  = a[0]*b[1] + a[1]*b[5] + a[2]*b[9] + a[3]*b[13];
  retmat[2]  = a[0]*b[2] + a[1]*b[6] + a[2]*b[10] + a[3]*b[14];
  retmat[3]  = a[0]*b[3] + a[1]*b[7] + a[2]*b[11] + a[3]*b[15];

  retmat[4]  = a[4]*b[0] + a[5]*b[4] + a[6]*b[8] + a[7]*b[12];
  retmat[5]  = a[4]*b[1] + a[5]*b[5] + a[6]*b[9] + a[7]*b[13];
  retmat[6]  = a[4]*b[2] + a[5]*b[6] + a[6]*b[10] + a[7]*b[14];
  retmat[7]  = a[4]*b[3] + a[5]*b[7] + a[6]*b[11] + a[7]*b[15];

  retmat[8]  = a[8]*b[0] + a[9]*b[4] + a[10]*b[8] + a[11]*b[12];
  retmat[9]  = a[8]*b[1] + a[9]*b[5] + a[10]*b[9] + a[11]*b[13];
  retmat[10] = a[8]*b[2] + a[9]*b[6] + a[10]*b[10] + a[11]*b[14];
  retmat[11] = a[8]*b[3] + a[9]*b[7] + a[10]*b[11] + a[11]*b[15];

  retmat[12] = a[12]*b[0] + a[13]*b[4] + a[14]*b[8] + a[15]*b[12];
  retmat[13] = a[12]*b[1] + a[13]*b[5] + a[14]*b[9] + a[15]*b[13];
  retmat[14] = a[12]*b[2] + a[13]*b[6] + a[14]*b[10] + a[15]*b[14];
  retmat[15] = a[12]*b[3] + a[13]*b[7] + a[14]*b[11] + a[15]*b[15];

}


void transposeMat4f( float* a, float *retmat ){
  retmat[0]  = a[0];
  retmat[1]  = a[4];
  retmat[2]  = a[8];
  retmat[3]  = a[11];

  retmat[4]  = a[1];
  retmat[5]  = a[5];
  retmat[6]  = a[9];
  retmat[7]  = a[13];

  retmat[8]  = a[2];
  retmat[9]  = a[6];
  retmat[10] = a[10];
  retmat[11] = a[14];

  retmat[12] = a[3];
  retmat[13] = a[7];
  retmat[14] = a[11];
  retmat[15] = a[15];
}



Vec4 add4( Vec4 a, Vec4 b ) {
  return (Vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

Vec4 subtract4( Vec4 a,Vec4  b ) {
  return (Vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

