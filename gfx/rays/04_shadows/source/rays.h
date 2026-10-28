#ifndef _RAYS_H_
#define _RAYS_H_

#include "my_math.h"
#include "canvas.h"
#include "light.h"

#define VIEW_WIDTH  1
#define VIEW_HEIGHT 1
#define VIEW_DISTANCE 1

typedef struct {
  Vec3 center;
  float radius;
  int16_t color;
  int16_t specular;
} Sphere;

#define NUM_SPHERES 4
#define NUM_LIGHTS  3
typedef struct {
  Sphere spheres[NUM_SPHERES];
  Light  lights[NUM_LIGHTS];
} Scene;

void init_scene();

// x, y are canvas coordinates. returns 3d vector for viewport coord
Vec3  canvas_to_viewport( float x, float y);

// O - camera
// D - direction 
// t_min, t_max - range of t we use 
// RETURNS: color as 16 bit int
int16_t trace_ray( Vec3 O, Vec3 D, float t_min, float t_max );

// O - camera
// D - direction 
// S - Sphere we're checking for intersection
// flase if no intersection. Sets  t1 and t2 if found
bool intersect_ray_sphere( Vec3 O, Vec3 D, Sphere S, float *t1, float *t2 );

// P - point on sphere we intersected
// N - normal on sphere at intersection
// V - view vector pointing from P to camera 
// s - specular intensity
// returns lighting scalar
float compute_lighting( Vec3 P, Vec3 N, Vec3 V, int16_t s );

// O - camera
// D - direction 
// t_min, t_max - range of t we use 

// RETURNS: index of closest sphere.(-1 none found).  t_closest is closest sphere
int closest_intersection( Vec3 O, Vec3 D, float t_min, float t_max, float *t_closest );

//  P -point on object 
//  N - Normal
//  V - view angle

#endif // _RAYS_H_
