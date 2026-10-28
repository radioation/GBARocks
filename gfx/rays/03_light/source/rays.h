#ifndef _RAYS_H_
#define _RAYS_H_

#include "my_math.h"
#include "canvas.h"

#define VIEW_WIDTH  1
#define VIEW_HEIGHT 1
#define VIEW_DISTANCE 1

typedef struct {
  Vec3 center;
  float radius;
  int16_t color;
} sphere;


extern sphere spheres[];
Vec3  canvas_to_viewport( float x, float y);
int16_t trace_ray( Vec3 O, Vec3 D, float t_min, float t_max );
bool intersect_ray_sphere( Vec3 O, Vec3 D, sphere S, float *t1, float *t2 );

#endif // _RAYS_H_
