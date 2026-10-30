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

