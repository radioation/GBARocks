
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_math.h"
#include "rays.h"
#include <float.h>

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

  // set graphics mode
  SetMode( MODE_3 | BG2_ON );

  init_scene();
  // tracer
  Vec3 O =(Vec3) {0.0f, 0.0f, 0.0f};
  for( int x = -CANVAS_WIDTH/2; x < CANVAS_WIDTH/2; ++x ) {
    for( int y = -CANVAS_HEIGHT/2; y < CANVAS_HEIGHT/2; ++y ) {
      Vec3 D = canvas_to_viewport( x,y);
      u16 color = trace_ray( O, D, 1, FLT_MAX );
      put_pixel(x, y, color );
    }
  }

  while (1) {
    VBlankIntrWait();
  }
}


