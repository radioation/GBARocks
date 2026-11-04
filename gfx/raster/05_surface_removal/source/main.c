
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "canvas.h"
#include "my_math.h"
#include "raster.h"

#define RED RGB5( 31,0,0 )
#define GREEN RGB5( 0,31,0 )
#define BLUE RGB5( 0,0,31 )
#define YELLOW RGB5( 31,31,0)
#define PURPLE RGB5( 31,0,31)
#define CYAN RGB5( 0,31,31)

bool transform_and_clip( Plane *clipping_planes, Model *model, float scale, float* transform, Model *new_model ) {
  // TAC _transform and clip
  //   let clipped = TransformAndClip(camera.clipping_planes, instances[i].model, instances[i].scale, transform);
  // TAC- transform bounding sphere. and look for early discard.
  Vec4 center = mat4fXvec4f( transform, model->bounds_center );
  for( int p=0; p < 5; p++ ) {
    Vec3 center3;
    center3.x = center.x;
    center3.y = center.y;
    center3.z = center.z;
    float distance = dot3( clipping_planes[p].normal, center3 ) + clipping_planes[p].distance;
    if( distance <  model->bounds_radius ) {
      return false;
    } 

  }
  // TAC- Apply modelview transform
  new_model->vertices = malloc( sizeof(  Vec4 )  * model->num_vertices * 2 ); // doubling space to avoid realloc latter.
  new_model->num_vertices = 0;
  for (int  i = 0; i < model->num_vertices; i++) {
    Vec3 newvert = model->vertices[i];
    Vec4 tmpvert;
    tmpvert.x = newvert.x;
    tmpvert.y = newvert.y;
    tmpvert.z = newvert.z;
    tmpvert.w = 1.0;
    Vec4 newvert4 = mat4fXvec4f(transform,  tmpvert );
    new_model->vertices[i].x = newvert4.x;
    new_model->vertices[i].y = newvert4.y;
    new_model->vertices[i].z = newvert4.z;
    new_model->num_vertices++;
  }

  // TAC- clip entire model against each frustum plane

  //let triangles = model.triangles.slice(); // full copy of triangles.
  //new_model->triangles = malloc( sizeof(  Vec4 )  * model->num_triangles * 2 ); // doubling space to avoid realloc latter.
  Triangle *oldtri  = malloc( sizeof( Triangle )  * model->num_triangles );
  int oldtri_count = model->num_triangles;
  memcpy( oldtri, model->triangles, sizeof(Triangle) * model->num_triangles);
  Triangle *newtri  = 0;
  int newtri_count = 0;

  
  for (int p = 0; p < 5; p++) {
    if(newtri) free(newtri );
    newtri  = malloc( sizeof(  Triangle )  * oldtri_count *2  ); // doubling to avoid dynamic realloc in the loop
    newtri_count = 0;
    for (int i = 0; i < oldtri_count; i++) {  
      //ClipTriangle(triangles[i], clipping_planes[p], new_triangles, vertices);
      Vec3 v0 = new_model->vertices[model->triangles[i].v0];
      Vec3 v1 = new_model->vertices[model->triangles[i].v1];
      Vec3 v2 = new_model->vertices[model->triangles[i].v2];

      int in0 = dot3( clipping_planes[p].normal, v0 ) + clipping_planes[p].distance > 0 ? 1 : 0;
      int in1 = dot3( clipping_planes[p].normal, v1 ) + clipping_planes[p].distance > 0 ? 1 : 0;
      int in2 = dot3( clipping_planes[p].normal, v2 ) + clipping_planes[p].distance > 0 ? 1 : 0;
      int in_count = in0+ in1+ in2;
      if( in_count == 0 ) {
        // do nothing. triangle is fully clipped out
      } else if ( in_count == 3 ) {
        //fully in
        newtri[newtri_count] = oldtri[i];
        newtri_count ++;
      } else if ( in_count == 1 ) {
        // one vertex in, output is one clipped triangle
        // TODO (but probably never): get new two new vertices from intersection, add to new vertex list
        if( in0 ) {
        }else if ( in1 ) {
        }else if ( in2 ) {
        }
        //newtri_count ++;
        // set triangle with new vertex indices.
      } else if ( in_count == 2 ) {
        // TODO (but probably never): bisect into two new triangles.
        if( in0 ) {
        }else if ( in1 ) {
        }else if ( in2 ) {
        }

        //newtri_count +=2;
      }

    }
    free(oldtri);
    oldtri  = malloc( sizeof( Triangle )  * newtri_count );
    oldtri_count = newtri_count;
    memcpy( oldtri, newtri, sizeof(Triangle) * newtri_count);
  }
  new_model->triangles  = malloc( sizeof( Triangle )  * newtri_count );
  memcpy( new_model->triangles, newtri, sizeof(Triangle) * newtri_count);

  new_model->num_triangles = newtri_count;
  new_model->bounds_center = center;
  new_model->bounds_radius = 1.73205080757;  // sqrt(3) 1.73205080757
  return true;
}


//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
  //---------------------------------------------------------------------------------


  // the vblank interrupt must be enabled for VBlankIntrWait() to work
  // since the default dispatcher handles the bios flags no vblank handler
  // is required
  irqInit();
  irqEnable(IRQ_VBLANK);


  // 
  SetMode( MODE_3 | BG2_ON );
  clear_zbuffer();

  // setup model
  // hardcoded 8 vertices.
  Vec3 vertices[] = {
    {1, 1, 1},
    {-1, 1, 1},
    {-1, -1, 1},
    {1, -1, 1},
    {1, 1, -1},
    {-1, 1, -1},
    {-1, -1, -1},
    {1, -1, -1},
  };

  Triangle triangles[] = {
    {0, 1, 2, RED},
    {0, 2, 3, RED},
    {1, 5, 6, YELLOW},
    {1, 6, 2, YELLOW},
    {2, 6, 7, CYAN},
    {2, 7, 3, CYAN},
    {4, 0, 3, GREEN},
    {4, 1, 0, PURPLE},
    {4, 3, 7, GREEN},
    {4, 5, 1, PURPLE},
    {5, 4, 7, BLUE},
    {5, 7, 6, BLUE},
  };
  // hardcoded 12 triangles.
  Model cube = { vertices, triangles, 8, 12, {0.0f, 0.0f, 0.0f, 1.0f}, 1.73205080757};  // sqrt(3) 1.73205080757

  // setup instances.
  Instance instances[] = {
    { &cube, { -1.5, 0.0, 7.0}, {0 }, 0.75, {} },
    { &cube, { 1.25, 2.5, 7.5}, {0 }, 1.00, {} },
//    { &cube, { 0.0, 0.0, -10.0}, {0 }, 1.00, {} },  Don't bother with this example.

  };
  get_identity_matrix( instances[0].orientation);
  get_instance_transform( &instances[0] );

  get_y_rotation_matrix( 195, instances[1].orientation);
  get_instance_transform( &instances[1] );

  get_y_rotation_matrix( 195, instances[2].orientation);
  get_instance_transform( &instances[2] );

  // setup camera
  float degrees = -30.0f;
  Camera cam;
  cam.position = (Vec3) {-3, 1, 2 };
  get_y_rotation_matrix( degrees, cam.orientation );

  float s2 =  0.70710678118; // 1/sqrt(2)
  cam.clipping_planes[0] = (Plane){ { 0.0, 0.0, 1 }, -1 }; // near 
  cam.clipping_planes[1] = (Plane){ { s2, 0.0, s2 }, 0 }; // left
  cam.clipping_planes[2] = (Plane){ { -s2, 0.0, s2 }, 0 }; // right
  cam.clipping_planes[3] = (Plane){ { 0.0, -s2, s2 }, 0 }; // top
  cam.clipping_planes[4] = (Plane){ { 0.0, s2, s2 }, 0 }; // Bottom

  //  setup camera_matrix
  float camera_translation[16];
  get_translation_matrix( mul3(cam.position, -1.0f), camera_translation );
  float camera_orientation_t[16];
  get_transpose_matrix( cam.orientation, camera_orientation_t );
  float camera_matrix[16];
  mat4fXmat4f( camera_orientation_t, camera_translation, camera_matrix );


  // iter instances
  for( int i=0; i < 2; ++i ) {
    // get transform
    float transform[16];
    mat4fXmat4f( camera_matrix,instances[i].transform,  transform );

    Model new_model;
    bool res = transform_and_clip( cam.clipping_planes, instances[i].model, instances[i].scale, transform, &new_model ) ;

    if (res) {
      // RenderModel()
      //  render vertices
      Point2 projected[16]; // figure this out.
      size_t curr_p = 0;
      for( int j=0; j < new_model.num_vertices; ++j ) {
        Vec4 vert;
        vert.x = new_model.vertices[j].x;
        vert.y = new_model.vertices[j].y;
        vert.z = new_model.vertices[j].z;
        vert.w = 1.0;
     //   Vec4 vert = mat4fXvec4f( transform, vertH );
        projected[curr_p] = project_vertex4( vert );
        curr_p ++;
      }
      for( int j=0; j < new_model.num_triangles; ++j ) {
        // render triangle
        //draw_wireframe_triangle(
        //draw_filled_triangle(
        //    projected[ new_model.triangles[j].v0 ],
        //    projected[ new_model.triangles[j].v1 ],
        //    projected[ new_model.triangles[j].v2],
        //    new_model.triangles[j].color
        //    );
        draw_triangle( new_model.triangles[j], projected, new_model.vertices );
      }

    }
  }



  while (1) {
    VBlankIntrWait();
  }
}


