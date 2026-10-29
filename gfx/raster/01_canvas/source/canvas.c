

#include "canvas.h"

static float vals[CANVAS_WIDTH];

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

void draw_line( Point2 p0, Point2 p1, uint16_t color ) {
  if ( abs( p1.x - p0.x ) > abs( p1.y - p0.y ) ) {
    // line is horizontal~ish
    // make sure x0 < x1
    if( p0.x > p1.x ) {
      Point2 temp = p0;
      p0 = p1;
      p1 = temp;
    }
    interpolate( p0.x, p0.y, p1.x, p1.y, vals );
    for(int x = p0.x; x <= p1.x; x++  ) {
      put_pixel( x, vals[x - (int)p0.x], color );
    }
  } else {
    if( p0.y > p1.y ) {
      // swap
      Point2 temp = p0;
      p0 = p1;
      p1 = temp;
    }
    interpolate( p0.y, p0.x, p1.y, p1.x, vals );
    for(int y = p0.y; y <= p1.y; y++  ) {
      put_pixel( vals[y - (int)p0.y], y, color );
    }
  }
}








