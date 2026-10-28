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
} Sphere;

#define NUM_SPHERES 4
#define NUM_LIGHTS  3
typedef struct {
  Sphere spheres[NUM_SPHERES];
  Light  lights[NUM_LIGHTS];
} Scene;

void init_scene();

Vec3  canvas_to_viewport( float x, float y);
int16_t trace_ray( Vec3 O, Vec3 D, float t_min, float t_max );
bool intersect_ray_sphere( Vec3 O, Vec3 D, Sphere S, float *t1, float *t2 );
float compute_lighting( Vec3 P, Vec3 N );

#endif // _RAYS_H_
