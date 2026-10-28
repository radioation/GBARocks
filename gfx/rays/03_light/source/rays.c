#include <float.h>
#include <math.h>
#include "rays.h"
#include "canvas.h"


Scene scene;

void init_scene() {
  scene.spheres[0] = (Sphere){ .center = {0,-1,3}, .radius = 1.0, .color = RGB5(31,0,0 )};
  scene.spheres[1] = (Sphere){ .center = {2, 0, 4}, .radius = 1.0, .color = RGB5(0,0,31 )};
  scene.spheres[2] = (Sphere) { .center = {-2,0, 4}, .radius = 1.0, .color = RGB5(0,31,0 )};
  scene.spheres[3] = (Sphere) { .center = {0, -5001, 0}, .radius = 5000, .color = RGB5(31,31,0 )};
  scene.lights[0] = (Light) { .type = AMBIENT, .intensity = 0.2, {} };
  scene.lights[1] = (Light) { .type = POINT, .intensity = 0.6, .position = (Vec3) { 2,1,0 }, .direction = {} };
  scene.lights[2] = (Light) { .type = DIRECTIONAL, .intensity = 0.2, .position = {},  .direction = (Vec3) {1,4,4}};
};

Vec3  canvas_to_viewport( float x, float y ){
  return (Vec3) {  x * VIEW_WIDTH / CANVAS_WIDTH, y * VIEW_HEIGHT / CANVAS_HEIGHT, VIEW_DISTANCE };
}


int16_t trace_ray( Vec3 O, Vec3 D, float t_min, float t_max ) {
  float closest_t = FLT_MAX;
  //sphere *closest_sphere = NULL;
  int closest_sphere_index = -1;
  for( int i=0; i < NUM_SPHERES; i++ ) {
    float t1, t2;
    if( intersect_ray_sphere( O, D, scene.spheres[i], &t1, &t2 ) ){
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
  Vec3 P = add( O, mul( D, closest_t ) );
  Vec3 N = subtract( P, scene.spheres[closest_sphere_index].center );
  N = divide( N, length( N ) );
  float l =  compute_lighting( P, N );
  int r = RGB5_GET_RED(scene.spheres[closest_sphere_index].color)* l; 
  if( r > 31 ) r = 31;
  int g = RGB5_GET_GREEN(scene.spheres[closest_sphere_index].color)* l;
  if( g > 31 ) g = 31;
  int b = RGB5_GET_BLUE(scene.spheres[closest_sphere_index].color)* l;
  if( b > 31 ) b = 31;
  return RGB5( r, g, b);
}
bool intersect_ray_sphere( Vec3 O, Vec3 D, Sphere S, float *t1, float *t2 ) {
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

float compute_lighting( Vec3 P, Vec3 N ){
  float i = 0.0;
  for ( int j =0; j < NUM_LIGHTS; ++j ) {
    if( scene.lights[j].type == AMBIENT ) {
      i += scene.lights[j].intensity;
    } else {
      Vec3 L;
      if( scene.lights[j].type == AMBIENT ) {
        L = subtract( scene.lights[j].position, P );
      }else {
        L =scene.lights[j].direction;
      }
      float n_dot_l = dot( N, L );
      if ( n_dot_l > 0 ) {
        i += scene.lights[j].intensity * n_dot_l / ( length(N) * length(L ) );
      }
    }
  }
  return i;

}
