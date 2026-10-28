
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
    put_pixel( 110, -70, RGB5( 310, 31, 0) );

    while (1) {
        VBlankIntrWait();
    }
}


