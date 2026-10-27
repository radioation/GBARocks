
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

#define CANVAS_WIDTH 240
#define CANVAS_HEIGHT 160

volatile u16 *vram = (volatile u16 *)VRAM;

// put_pixel coordniate systme runs [ -CANVAS_WIDTH/2, CANVAS_WIDTH/2 ) left to right
// [ -CANVAS_HEIGHT/2, CANVAS_HEIGHT/2 ) Top to bottom.
static inline void put_pixel(s16 canvas_x, s16 canvas_y, u16 color)
{
    // 
    s16 screen_x = CANVAS_WIDTH/2 + canvas_x;
    s16 screen_y = CANVAS_HEIGHT/2 - canvas_y;
    
    if (screen_x < CANVAS_WIDTH  && screen_x >= 0 &&
        screen_y < CANVAS_HEIGHT && screen_y >= 0 )
    {
        vram[screen_y * CANVAS_WIDTH + screen_x] = color;
    }
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

    // get start of video ram

    // set every pixel to red
    for( int i=0; i < 240*160; ++i ) {
        vram[i] = RGB5( 0, 0, 8 );  
    }


    while (1) {
        VBlankIntrWait();
    }
}


