
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
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

  // setup model
  Vec3 verticies[] = {
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
  {4, 0, 3, GREEN},
  {4, 3, 7, GREEN},
  {5, 4, 7, BLUE},
  {5, 7, 6, BLUE},
  {1, 5, 6, YELLOW},
  {1, 6, 2, YELLOW},
  {4, 5, 1, PURPLE},
  {4, 1, 0, PURPLE},
  {2, 6, 7, CYAN},
  {2, 7, 3, CYAN}
  };
  Model cube = { verticies, triangles};

  // setup instances.
  Instance instances[] = {
    { &cube, { -1.5, 0.0, 7.0}, {0 }, 0.75 },
    { &cube, { 1.25, 2.5, 7.5}, {0 }, 1.25 },
  };
  get_identity_matrix( instances[0].orientation);
  get_y_rotation_matrix( 195, instances[1].orientation);


  // setup camera
  // let camera = new Camera(new Vertex(-3, 1, 2), MakeOYRotationMatrix(-30));
  float degrees = -30.0f;
  Camera cam;
  cam.position = (Vec3) { -3, 1, 2 };
  get_y_rotation_matrix( degrees, cam.orientation );


  //  setup camera_matrix
  float camera_translation[16];
  get_translation_matrix( mul3(cam.position, -1.0f), camera_translation );
  float camera_orientation_t[16];
  get_transpose_matrix( cam.orientation, camera_orientation_t );
  float camera_matrix[16];
  mat4fXmat4f( camera_orientation_t, camera_translation, camera_matrix );
 

  // iter instances
  for( int i=0; i < 2; ++i ) {
    // render
    float transform[16];
    mat4fXmat4f( camera_matrix,instance[i].transform,  transform );
    //  render vertices
    Point2 projected[ 512]; // figure this out.
    size_t curr_p = 0;
    for( int j=0; j < instance[i].model->num_verticies; ++j ) {
      Vec4 vertH ;
      vertH.x = instance[i].model->vertices[j].x;
      vertH.y = instance[i].model->vertices[j].y;
      vertH.z = instance[i].model->vertices[j].z;
      vertH.w = 1.0;
      Vec4 vert = mat4fXvec4f( instance[i].model.transform, vertH );
      projected[curr_p] = project_vert3ex( v );
      curr_p ++;
    }
    for( int j=0; j < instance[i].model->num_triangles; ++j ) {
      // render triangle
      draw_wireframe_triangle(
          projected[ instance[i].model->triangles[j].v0 ],
          projected[ instance[i].model->triangles[j].v1 ],
          projected[ instance[i].model->triangles[j].v22],
          instance[i].model->triangles[j].color ,
          );
    }

  }



  while (1) {
    VBlankIntrWait();
  }
}


