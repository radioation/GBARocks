#ifndef _RASTER_H_
#define _RASTER_H_
#include "my_math.h"
#include "canvas.h"


#define VIEW_WIDTH  1
#define VIEW_HEIGHT 1
#define VIEW_DISTANCE 1


typedef struct {
  uint16_t v0;
  uint16_t v1;
  uint16_t v2;
  uint16_t color;
} Triangle;

typedef struct {
  Vec4 *vertices;
  Triangle *triangles;
} Model;

typedef struct{
  Model model;
  Vec4 position;
  Vec4 orientation;
  float scale;
} Instance;

typedef struct {
  Vec3 position;
  float orientation[16];
}  Camera;

void get_translation_matrix( Vec3 t, float *mat );
void get_scaling_matrix( float s, float *mat );
void get_x_rotation_matrix( float s, float *mat );
void get_y_rotation_matrix( float s, float *mat );
void get_z_rotation_matrix( float s, float *mat );


Point2 viewport_to_canvas( float x, float y );
Point2 project_vertex( Vec3 v );



#endif // _RASTER_H_
