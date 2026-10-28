#include <float.h>
#include <math.h>
#include "rays.h"
#include "canvas.h"


sphere spheres[] = {
  { .center = {0,-1,1}, .radius = 1.0, .color = RGB5(31,0,0 )},
  { .center = {2, 0, 4}, .radius = 1.0, .color = RGB5(0,0,31 )},
  { .center = {-2,0, 4}, .radius = 1.0, .color = RGB5(0,31,0 )},
};

Vec3  canvas_to_viewport( float x, float y ){
  return (Vec3) {  x * VIEW_WIDTH / CANVAS_WIDTH, y * VIEW_HEIGHT / CANVAS_HEIGHT, VIEW_DISTANCE };
}


int16_t trace_ray( Vec3 O, Vec3 D, float t_min, float t_max ) {
  float closest_t = FLT_MAX;
  //sphere *closest_sphere = NULL;
  int closest_sphere_index = -1;
  for( int i=0; i < 3; i++ ) {
    float t1, t2;
    if( intersect_ray_sphere( O, D, spheres[i], &t1, &t2 ) ){
      if ( t1 > t_min && t1 < t_max && t1 < closest_t ) {
        closest_t = t1;
        closest_sphere_index = i;
      }
      if ( t2 > t_min && t2 < t_max && t2 < closest_t ) {
        closest_t = t2;
        closest_sphere_index = i;
      }
    }
  }

  if( closest_sphere_index < -1  ) {
    return RGB5(31,31,31);
  }
  return spheres[closest_sphere_index].color;

}
bool intersect_ray_sphere( Vec3 O, Vec3 D, sphere S, float *t1, float *t2 ) {
  float r= S.radius;
  Vec3 CO = subtract( O,  S.center );
  float a = dot( D, D );
  float b= 2 * dot( CO, D );
  float c = dot( CO, CO) - r*r;
  float discriminant = b*b - 4* a*c;
  if( discriminant < 0.0f) {
    return false;
  }
  *t1 = ( -b + sqrt( discriminant)) / (2*a);
  *t2 = ( -b - sqrt( discriminant)) / (2*a);
  return true;
}

