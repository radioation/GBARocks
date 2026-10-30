#define _USE_MATH_DEFINES
#include <math.h>
#include "raster.h"

void get_translation_matrix( Vec3 t, float *mat ) {
  mat[ 0]= 1.0; mat[ 1] = 0.0; mat[ 2] = 0.0; mat[ 3] = t.x;
  mat[ 4]= 0.0; mat[ 5] = 1.0; mat[ 6] = 0.0; mat[ 7] = t.y;
  mat[ 4]= 0.0; mat[ 5] = 0.0; mat[ 6] = 1.0; mat[ 7] = t.z;
  mat[ 4]= 0.0; mat[ 5] = 0.0; mat[ 6] = 0.0; mat[ 7] = 1.0;
}

void get_scaling_matrix( float  s, float *mat ) {
  mat[ 0]=   s; mat[ 1] = 0.0; mat[ 2] = 0.0; mat[ 3] = 0.0;
  mat[ 4]= 0.0; mat[ 5] =   s; mat[ 6] = 0.0; mat[ 7] = 0.0;
  mat[ 4]= 0.0; mat[ 5] = 0.0; mat[ 6] =   s; mat[ 7] = 0.0;
  mat[ 4]= 0.0; mat[ 5] = 0.0; mat[ 6] = 0.0; mat[ 7] = 1.0;
}

void get_x_rotation_matrix( float degrees, float *mat ) {
  float c = cos( degrees * M_PI / 180.0 );
  float s = sin( degrees * M_PI / 180.0 );
  mat[ 0]= 1.0; mat[ 1] = 0.0; mat[ 2] = 0.0; mat[ 3] = 0.0;
  mat[ 4]= 0.0; mat[ 5] =   c; mat[ 6] =  -s; mat[ 7] = 0.0;
  mat[ 4]= 0.0; mat[ 5] =   s; mat[ 6] =   c; mat[ 7] = 0.0;
  mat[ 4]= 0.0; mat[ 5] = 0.0; mat[ 6] = 0.0; mat[ 7] = 1.0;
}

void get_y_rotation_matrix( float degrees, float *mat ) {
  float c = cos( degrees * M_PI / 180.0 );
  float s = sin( degrees * M_PI / 180.0 );
  mat[ 0]=   c; mat[ 1] = 0.0; mat[ 2] =  -s; mat[ 3] = 0.0;
  mat[ 4]= 0.0; mat[ 5] = 1.0; mat[ 6] = 0.0; mat[ 7] = 0.0;
  mat[ 4]=   s; mat[ 5] = 0.0; mat[ 6] =   s; mat[ 7] = 0.0;
  mat[ 4]= 0.0; mat[ 5] = 0.0; mat[ 6] = 0.0; mat[ 7] = 1.0;
}


void get_z_rotation_matrix( float degrees, float *mat ) {
  float c = cos( degrees * M_PI / 180.0 );
  float s = sin( degrees * M_PI / 180.0 );
  mat[ 0]=   c; mat[ 1] =  -s; mat[ 2] = 0.0; mat[ 3] = 0.0;
  mat[ 4]=   s; mat[ 5] =   c; mat[ 6] = 0.0; mat[ 7] = 0.0;
  mat[ 4]= 0.0; mat[ 5] = 0.0; mat[ 6] = 1.0; mat[ 7] = 0.0;
  mat[ 4]= 0.0; mat[ 5] = 0.0; mat[ 6] = 0.0; mat[ 7] = 1.0;
}

void get_identity_matrix(  float *mat ) {
  mat[ 0]= 1.0; mat[ 1] = 0.0; mat[ 2] = 0.0; mat[ 3] = 0.0;
  mat[ 4]= 0.0; mat[ 5] = 1.0; mat[ 6] = 0.0; mat[ 7] = 0.0;
  mat[ 4]= 0.0; mat[ 5] = 0.0; mat[ 6] = 1.0; mat[ 7] = 0.0;
  mat[ 4]= 0.0; mat[ 5] = 0.0; mat[ 6] = 0.0; mat[ 7] = 1.0;
}

void get_transpose_matrix(  float *mat, float* transpose ) {
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // matrix[i][j]      -> matrix[i * 4 + j]
            // transpose[j][i]   -> transpose[j * 4 + i]
            transpose[j * 4 + i] = mat[i * 4 + j];
        }
    }
}

void get_instance_transform( Instance* inst ) {
//  MultiplyMM4(
//      MakeTranslationMatrix(this.position), 
//      MultiplyMM4(this.orientation, MakeScalingMatrix(this.scale))
//
//      );
  float scale[16];
  float temp[16];
  get_scaling_matrix( inst->scale, scale );
  mat4fXmat4f( inst->orientation, scale, temp );
  float transmat[16];
  get_translation_matrix( inst->position, transmat );
  mat4fXmat4f(transmat, temp, inst->transform );

}


Point2 viewport_to_canvas( float x, float y ) {
  return (Point2) { x * (float)CANVAS_WIDTH/(float)VIEW_WIDTH, y * (float)CANVAS_HEIGHT/(float)VIEW_HEIGHT };
}


Point2 project_vertex( Vec3 v ) {
  return viewport_to_canvas( v.x * (float)VIEW_DISTANCE / v.z, v.y * (float)VIEW_DISTANCE / v.z );
}

Point2 project_vertex4( Vec4 v ) {
  return viewport_to_canvas( v.x * (float)VIEW_DISTANCE / v.z, v.y * (float)VIEW_DISTANCE / v.z );
}


