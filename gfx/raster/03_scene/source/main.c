
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

  // let camera = new Camera(new Vertex(-3, 1, 2), MakeOYRotationMatrix(-30));
  float degrees = -30.0f;
  Camera cam;
  cam.position = (Vec3) { -3, 1, 2 };
  get_y_rotation_matrix( degrees, cam.orientation );


  // 1. model transform, move from model space to world space


  // 2 Camera Transform



  // 3. Perspective 

  // 4. Cavas
 
  while (1) {
    VBlankIntrWait();
  }
}


