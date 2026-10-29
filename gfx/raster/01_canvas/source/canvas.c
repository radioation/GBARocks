
#include <string.h>
#include "canvas.h"

static float vals[CANVAS_WIDTH];

volatile u16 *vram = (volatile u16 *)VRAM;  

#define swap(type,a,b) do { type _c; _c = a; a = b; b = _c; } while(0)


#include <stdio.h>
#include <stdio.h>

void debug_dump_float_array(const char* label, float* array, int length) {
    printf("\n--- DUMPING FLOAT ARRAY: %s ---\n", label);
    
    for (int i = 0; i < length; i++) {
        float val = array[i];
        
        // Handle negative numbers cleanly
        int is_negative = (val < 0) ? 1 : 0;
        if (is_negative) val = -val;

        // Extract whole number part
        int whole = (int)val;
        
        // Extract 4 decimal places of precision safely without relying on %f
        int fraction = (int)((val - whole) * 10000.0f + 0.5f); 

        // Print the index and formatted float data out to the terminal log
        printf("  [%d]: %s%d.%04d\n", 
               i, 
               is_negative ? "-" : "", 
               whole, 
               fraction);
    }
    printf("----------------------------------------\n");
}

void debug_dump_array(const char* label, int* array, int length) {
    volatile u16* mgba_log_reg = (u16*)0x04FFF000;
    volatile u16* mgba_flush_reg = (u16*)0x04FFF130;
    char buffer[128];

    // 1. Print the header label
    snprintf(buffer, sizeof(buffer), "--- DUMPING: %s ---", label);
    for (int i = 0; buffer[i] != '\0'; i++) mgba_log_reg[i] = buffer[i];
    *mgba_flush_reg = 0x0002 | 0x100; // Flush as warning log

    // 2. Loop through and log array values
    for (int i = 0; i < length; i++) {
        snprintf(buffer, sizeof(buffer), "  [%d]: %d (Hex: 0x%08X)", i, array[i], array[i]);
        for (int j = 0; buffer[j] != '\0'; j++) mgba_log_reg[j] = buffer[j];
        *mgba_flush_reg = 0x0002 | 0x100; // Flush row
    }
}


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

  float a = (d1 - d0) /(float) (i1 -i0);
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
      swap(Point2, p0, p1 );
    }
    interpolate( p0.x, p0.y, p1.x, p1.y, vals );
    for(int x = p0.x; x <= p1.x; x++  ) {
      put_pixel( x, vals[x - (int)p0.x], color );
    }
  } else {
    if( p0.y > p1.y ) {
      // swap
      swap(Point2, p0, p1 );
    }
    interpolate( p0.y, p0.x, p1.y, p1.x, vals );
    for(int y = p0.y; y <= p1.y; y++  ) {
      put_pixel( vals[y - (int)p0.y], y, color );
    }
  }
}


void draw_wireframe_triangle( Point2 p0, Point2 p1, Point2 p2, uint16_t color )
{
  draw_line( p0, p1, color );
  draw_line( p1, p2, color );
  draw_line( p2, p0, color );
}

static float x01[CANVAS_WIDTH];
static float x12[CANVAS_WIDTH];
static float x012[CANVAS_WIDTH];

static float x02[CANVAS_WIDTH];
void draw_filled_triangle( Point2 p0, Point2 p1, Point2 p2, uint16_t color )
{
  // sort points
  if( p1.y  < p0.y ) {
 //   swap( Point2, p1, p0 );
      Point2 temp = p0;
      p0 = p1;
      p1 = temp;
  }
  if( p2.y  < p0.y ) {
   // swap( Point2, p2, p0 );
      Point2 temp = p0;
      p0 = p2;
      p2 = temp;
  }
  if( p2.y  < p1.y ) {
   // swap( Point2, p2, p1 );
      Point2 temp = p1;
      p1 = p2;
      p2 = temp;
  }

  // get the X cordinates for each y of the triangle sides
  int x01count =  interpolate( p0.y, p0.x, p1.y, p1.x, x01 );
  int x12count =  interpolate( p1.y, p1.x, p2.y, p2.x, x12 );
  int x02count =  interpolate( p0.y, p0.x, p2.y, p2.x, x02 );
  memset( x012, 0, sizeof( x012 ) );
  if( x01count > 0 ) {
    memcpy( x012, x01, (x01count -1)*sizeof(float) );
    memcpy( x012 + (x01count-1)*sizeof(float), x12, x12count*sizeof(float) );
  } else {
    memcpy( x012,  x12, x12count*sizeof(float) );
  }


  // which side is left and whichis right?
  size_t m = (x01count + x12count -1 )/2;
  float* x_left = x012;
  float* x_right = x02;
  if( x02[m] < x012[m] ) {
    x_left = x02; 
    x_right = x012; 
  } else {
    x_left = x012; 
    x_right = x02; 
  }

  // draw the horizontal segments
  for( int y = (int)p0.y ; y <=(int) p2.y; ++y ) {
    for( int x =x_left[ y - (int)p0.y]; x <=x_right[ y -(int) p0.y]; ++x) {
      put_pixel( x,y, color );
    }
  }

}







