

#include "canvas.h"



volatile u16 *vram = (volatile u16 *)VRAM;  

inline void put_pixel(s16 canvas_x, s16 canvas_y, u16 color)
{
    // 
    s16 screen_x = CANVAS_WIDTH/2 + canvas_x;  // translate X
    s16 screen_y = CANVAS_HEIGHT/2 - canvas_y; // translate and flip Y
    
    if (screen_x < CANVAS_WIDTH  && screen_x >= 0 &&
        screen_y < CANVAS_HEIGHT && screen_y >= 0 )
    {
        vram[screen_y * CANVAS_WIDTH + screen_x] = color;
    }
}



