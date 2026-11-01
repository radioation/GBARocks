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
  Vec3 *vertices;
  Triangle *triangles;
  int num_verticies;
  int num_triangles;
} Model;

typedef struct{
  Model *model;
  Vec3 position;
  float orientation[16];
  float scale;
  float transform[16];
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
void get_identity_matrix( float *mat );
void get_transpose_matrix( float *in, float* transpose  );

void get_instance_transform( Instance* inst );

Point2 viewport_to_canvas( float x, float y );
Point2 project_vertex( Vec3 v );
Point2 project_vertex4( Vec4 v );



#endif // _RASTER_H_
