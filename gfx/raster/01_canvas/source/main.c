
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include "canvas.h"




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



    // set every pixel to blue 
    for( int i=0; i < 240*160; ++i ) {
        vram[i] = RGB5( 0, 0, 8 );  
    }

    // Upper Left
    put_pixel( -110, 70, RGB5( 0, 31 ,0) );

    // Upper Right 
    put_pixel( 110, 70, RGB5( 0, 0 ,31) );

    // Lower Left
    put_pixel( -110, -70, RGB5( 31, 0 ,0) );

    // Lower Right 
    put_pixel( 110, -70, RGB5( 31, 31, 0) );

    // try out lines
    Point2 p0 = (Point2) { -CANVAS_WIDTH/2 + 20, -CANVAS_HEIGHT/2 + 20 };
    Point2 p1 = (Point2) { CANVAS_WIDTH/2 - 20, CANVAS_HEIGHT/2 -20};

    draw_line( p0, p1, RGB5( 31, 31, 31  ) );

    // draw triangle
     p0 = (Point2) { -60, -60 };
    p1 = (Point2) { -10, -60 };
    Point2 p2 = (Point2) { -10, -10 };
    draw_wireframe_triangle( p0, p1, p2, RGB5( 31, 0, 31 ));

    while (1) {
        VBlankIntrWait();
    }
}


