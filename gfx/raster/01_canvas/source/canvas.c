

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



int interpolate( int16_t i0, float d0, int16_t i1, float d1, float values[] ) {

  int count = 0; // not really needed? caller should know size already

  if( i0 == i1 ) {
    values[0] = d0;
    count = 1;
    return count;
  }

  float a = (d1 - d0) /(i1 -i0);
  float d = d0;
  // convetion for thebook includes i1
  for( int i= i0, j=0; i <= i1; ++i,++j) {
    count++;
    values[j] = d;
    d += a;
  }
  return count;
}


