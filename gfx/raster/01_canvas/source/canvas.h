#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_math.h"


#define CANVAS_WIDTH 240
#define CANVAS_HEIGHT 160

extern volatile u16 *vram;

// Following book convention for now:
// put_pixel coordniate systme runs [ -CANVAS_WIDTH/2, CANVAS_WIDTH/2 ) left to right
// [ -CANVAS_HEIGHT/2, CANVAS_HEIGHT/2 ) Bottom to top (flipping Y)
void put_pixel(s16 canvas_x, s16 canvas_y, u16 color);
int interpolate( int16_t i0, float d0, int16_t i1, float d1, float values[] );
void draw_line( Point2 p0, Point2 p1, uint16_t color );
void draw_wireframe_triangle( Point2 p0, Point2 p1, Point2 p2, uint16_t color );
void draw_filled_triangle( Point2 p0, Point2 p1, Point2 p2, uint16_t color );

#endif // _CANVAS_H_
