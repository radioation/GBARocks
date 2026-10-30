
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

  // front verticies
  Vec3 vAf = (Vec3) { -0.5,  1.0, 5.0 };
  Vec3 vBf = (Vec3) {  1.5,  1.0, 5.0 };
  Vec3 vCf = (Vec3) {  1.5, -1.0, 5.0 };
  Vec3 vDf = (Vec3) { -0.5, -1.0, 5.0 };

  // back verticies
  Vec3 vAb = (Vec3) {  -0.5,  1.0, 6.0 };
  Vec3 vBb = (Vec3) {   1.5,  1.0, 6.0 };
  Vec3 vCb = (Vec3) {   1.5, -1.0, 6.0 };
  Vec3 vDb = (Vec3) {  -0.5, -1.0, 6.0 };

  // draw back face
  draw_line( project_vertex( vAb ), project_vertex( vBb ), RGB5(31,0,0) );
  draw_line( project_vertex( vBb ), project_vertex( vCb ), RGB5(31,0,0) );
  draw_line( project_vertex( vCb ), project_vertex( vDb ), RGB5(31,0,0) );
  draw_line( project_vertex( vDb ), project_vertex( vAb ), RGB5(31,0,0) );

  // draw edges
  draw_line( project_vertex( vAf ), project_vertex( vAb ), RGB5(0,31,0) );
  draw_line( project_vertex( vBf ), project_vertex( vBb ), RGB5(0,31,0) );
  draw_line( project_vertex( vCf ), project_vertex( vCb ), RGB5(0,31,0) );
  draw_line( project_vertex( vDf ), project_vertex( vDb ), RGB5(0,31,0) );

  // draw front face
  draw_line( project_vertex( vAf ), project_vertex( vBf ), RGB5(0,0,31) );
  draw_line( project_vertex( vBf ), project_vertex( vCf ), RGB5(0,0,31) );
  draw_line( project_vertex( vCf ), project_vertex( vDf ), RGB5(0,0,31) );
  draw_line( project_vertex( vDf ), project_vertex( vAf ), RGB5(0,0,31) );

  // front verticies
  vAf = (Vec3) { -2.0, -0.5, 5.0 };
  vBf = (Vec3) { -2.0,  0.5, 5.0 };
  vCf = (Vec3) { -1.0,  0.5, 5.0 };
  vDf = (Vec3) { -1.0, -0.5, 5.0 };

  // back verticies
  vAb = (Vec3) { -2.0, -0.5, 6.0 };
  vBb = (Vec3) { -2.0,  0.5, 6.0 };
  vCb = (Vec3) { -1.0,  0.5, 6.0 };
  vDb = (Vec3) { -1.0, -0.5, 6.0 };

  // draw back face
  draw_line( project_vertex( vAb ), project_vertex( vBb ), RGB5(31,0,0) );
  draw_line( project_vertex( vBb ), project_vertex( vCb ), RGB5(31,0,0) );
  draw_line( project_vertex( vCb ), project_vertex( vDb ), RGB5(31,0,0) );
  draw_line( project_vertex( vDb ), project_vertex( vAb ), RGB5(31,0,0) );

  // draw edges
  draw_line( project_vertex( vAf ), project_vertex( vAb ), RGB5(0,31,0) );
  draw_line( project_vertex( vBf ), project_vertex( vBb ), RGB5(0,31,0) );
  draw_line( project_vertex( vCf ), project_vertex( vCb ), RGB5(0,31,0) );
  draw_line( project_vertex( vDf ), project_vertex( vDb ), RGB5(0,31,0) );

  // draw front face
  draw_line( project_vertex( vAf ), project_vertex( vBf ), RGB5(0,0,31) );
  draw_line( project_vertex( vBf ), project_vertex( vCf ), RGB5(0,0,31) );
  draw_line( project_vertex( vCf ), project_vertex( vDf ), RGB5(0,0,31) );
  draw_line( project_vertex( vDf ), project_vertex( vAf ), RGB5(0,0,31) );

  while (1) {
    VBlankIntrWait();
  }
}


