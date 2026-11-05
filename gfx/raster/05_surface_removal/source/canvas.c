
#include <string.h>
#include <float.h>

#include "canvas.h"

static float vals[512];
static float zbuffer[CANVAS_WIDTH * CANVAS_HEIGHT] __attribute__((section(".ewram"), nobss));

volatile uint16_t *vram = (volatile uint16_t *)VRAM;  

#define swap(type,a,b) do { type _c; _c = a; a = b; b = _c; } while(0)

#define RGB5_GET_RED(color)   ((color) & 0x1F)
#define RGB5_GET_GREEN(color) (((color) >> 5) & 0x1F)
#define RGB5_GET_BLUE(color)  (((color) >> 10) & 0x1F)


void clear_zbuffer() {
  for (int i = 0; i < 240 * 160; i++) {
    zbuffer[i] = -FLT_MAX;
  }
}

inline void put_pixel(int16_t canvas_x, int16_t canvas_y, uint16_t color)
{
  // 
  int16_t screen_x = CANVAS_WIDTH/2 + canvas_x;  // translate X
  int16_t screen_y = CANVAS_HEIGHT/2 - canvas_y; // translate and flip Y

  if (screen_x < CANVAS_WIDTH  && screen_x >= 0 &&
      screen_y < CANVAS_HEIGHT && screen_y >= 0 )
  {
    vram[screen_y * CANVAS_WIDTH + screen_x] = color;
  }
}



int interpolate( int16_t  i0, float d0, int16_t  i1, float d1, float values[] ) {

  int count = 0; // not really needed? caller should know size already

  if( i0 == i1 ) {
    values[0] = d0;
    count = 1;
    return count;
  }

  float a = (d1 - d0) /(float) (i1 -i0);
  float d = d0;
  // convention for the book includes i1
  for( int i= i0, j=0; i <= i1; ++i,++j) {
    count++;
    values[j] = d;
    d += a;
  }
  return count;
}

void draw_line( Point2 p0, Point2 p1, uint16_t  color ) {
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


void draw_wireframe_triangle( Point2 p0, Point2 p1, Point2 p2, uint16_t  color )
{
  draw_line( p0, p1, color );
  draw_line( p1, p2, color );
  draw_line( p2, p0, color );
}

static float x01[512];
static float x12[512];
static float x012[512];

static float x02[512];
void draw_filled_triangle( Point2 p0, Point2 p1, Point2 p2, uint16_t  color )
{
  // sort points
  if( p1.y  < p0.y ) {
    swap( Point2, p1, p0 );
  }
  if( p2.y  < p0.y ) {
    swap( Point2, p2, p0 );
  }
  if( p2.y  < p1.y ) {
    swap( Point2, p2, p1 );
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

static float iz01[512];
static float iz02[512];
static float iz12[512];
static float iz012[512];
static float zscan[512];
void draw_triangle( Triangle triangle, Point2 projected[], Vec3 vertices[] )
{
  // Use sort on triangle lookups to rearrange Y's
  // *IMP* these indices are NOT going to be CW and should not be used for finding the Normal
  int i0 = triangle.v0;
  int i1 = triangle.v1;
  int i2 = triangle.v2;


  Point2 p0 = projected[ i0 ];  
  Point2 p1 = projected[ i1 ];  
  Point2 p2 = projected[ i2 ];  
  // sort points
  if( p1.y  < p0.y ) {
    swap( Point2, p1, p0 );
    swap( int, i1, i0 );
  }
  if( p2.y  < p0.y ) {
    swap( Point2, p2, p0 );
    swap( int, i2, i0 );
  }
  if( p2.y  < p1.y ) {
    swap( Point2, p2, p1 );
    swap( int, i2, i1 );
  }
  //

  // backface culling
  // get triangle normal (use original indicies which are CW order)
  Vec3 v0 = vertices[triangle.v0];
  Vec3 v1 = vertices[triangle.v1];
  Vec3 v2 = vertices[triangle.v2];
  Vec3 v0v1 = subtract3( v1, v0 ); 
  Vec3 v0v2 = subtract3( v2, v0 ); 
  Vec3 normo = cross3( v0v1, v0v2 );
  Vec3  vertex_to_camera = mul3( v0, -1.0f );  // should be subtract camera.position, 
  float dotty =  dot3( vertex_to_camera, normo );
  if ( dotty <= 0.0f ) {
    return; // facing the other way, don't bother drawing.
  }
  // now use re-ordered indicies
  v0 = vertices[i0];
  v1 = vertices[i1];
  v2 = vertices[i2];
  p0 = projected[ i0 ];  
  p1 = projected[ i1 ];  
  p2 = projected[ i2 ];  

  // get the X cordinates for each y of the triangle sides
  int x01count = interpolate( p0.y, p0.x,     p1.y, p1.x,     x01 );
  int z01count = interpolate( p0.y, 1.0/v0.z, p1.y, 1.0/v1.z, iz01 );

  int x12count = interpolate( p1.y, p1.x,     p2.y, p2.x,     x12 );
  int z12count = interpolate( p1.y, 1.0/v1.z, p2.y, 1.0/v2.z, iz12 );

  int x02count = interpolate( p0.y, p0.x,     p2.y, p2.x,     x02 );
  int z02count = interpolate( p0.y, 1.0/v0.z, p2.y, 1.0/v2.z, iz02 );

  memset( x012, 0, sizeof( x012 ) );
  memset( iz012, 0, sizeof( iz012 ) );
  if( x01count > 0 ) {
    memcpy( x012, x01, (x01count -1)*sizeof(float) );
    memcpy( x012 + (x01count-1), x12, x12count*sizeof(float) );

    memcpy( iz012, iz01, (z01count -1)*sizeof(float) );
    memcpy( iz012 + (z01count-1), iz12, z12count*sizeof(float) );
  } else {
    memcpy( x012,  x12, x12count*sizeof(float) );
    memcpy( iz012,  iz12, x12count*sizeof(float) );
  }


  // which side is left and which is right?
  size_t m = (x01count + x12count -1 )/2;
  float* x_left = x012;
  float* x_right = x02;
  float* iz_left = x012;
  float* iz_right = x02;
  if( x02[m] < x012[m] ) {
    x_left = x02; 
    x_right = x012; 
    iz_left = iz02; 
    iz_right = iz012; 
  } else {
    x_left = x012; 
    x_right = x02; 
    iz_left = iz012; 
    iz_right = iz02; 
  }


  // draw the horizontal segments
  for( int y = (int)p0.y ; y <=(int) p2.y; ++y ) {
    float xl = x_left[y - (int)p0.y];
    float xr = x_right[y - (int)p0.y];
    // interpolate  attributes for this scanline
    float  zl = iz_left[y - (int)p0.y];
    float  zr = iz_right[y - (int)p0.y];
    interpolate( xl, zl, xr, zr,  zscan );
    for( int x = xl; x <= xr; x++ ) {
      float iz = zscan[ x -(int) xl];
      int16_t screen_x = CANVAS_WIDTH/2 + x;  // translate X
      int16_t screen_y = CANVAS_HEIGHT/2 - y; // translate and flip Y
      if( zbuffer[ screen_x +screen_y * CANVAS_WIDTH ] < iz  ) {



        zbuffer[screen_x+screen_y*CANVAS_WIDTH] = iz;
        put_pixel( x,y, triangle.color );
      }

    }
  }

}


static float h01[CANVAS_WIDTH];
static float h12[CANVAS_WIDTH];
static float h012[CANVAS_WIDTH];

static float h02[CANVAS_WIDTH];
static float h[CANVAS_WIDTH];
void draw_shaded_triangle( Vec3 p0, Vec3 p1, Vec3 p2, uint16_t  color )
{
  // sort points
  if( p1.y  < p0.y ) {
    swap( Vec3, p1, p0 );
  }
  if( p2.y  < p0.y ) {
    swap( Vec3, p2, p0 );
  }
  if( p2.y  < p1.y ) {
    swap( Vec3, p2, p1 );
  }

  // get the X cordinates for each y of the triangle sides
  int x01count =  interpolate( p0.y, p0.x, p1.y, p1.x, x01 );
  int h01count =  interpolate( p0.y, p0.z, p1.y, p1.z, h01 );
  int x12count =  interpolate( p1.y, p1.x, p2.y, p2.x, x12 );
  int h12count =  interpolate( p1.y, p1.z, p2.y, p2.z, h12 );

  int x02count =  interpolate( p0.y, p0.x, p2.y, p2.x, x02 );
  int h02count =  interpolate( p0.y, p0.z, p2.y, p2.z, h02 );
  memset( x012, 0, sizeof( x012 ) );
  if( x01count > 0 ) {
    memcpy( x012, x01, (x01count -1)*sizeof(float) );
    memcpy( x012 + (x01count-1)*sizeof(float), x12, x12count*sizeof(float) );

    memcpy( h012, h01, (h01count -1)*sizeof(float) );
    memcpy( h012 + (h01count-1)*sizeof(float), h12, h12count*sizeof(float) );

  } else {
    memcpy( x012,  x12, x12count*sizeof(float) );
    memcpy( h012,  h12, h12count*sizeof(float) );
  }


  // which side is left and whichis right?
  size_t m = (x01count + x12count -1 )/2;
  float* x_left = x012;
  float* x_right = x02;
  float* h_left = h012;
  float* h_right = h02;
  if( x02[m] < x012[m] ) {
    x_left = x02; 
    x_right = x012; 
    h_left = h02; 
    h_right = h012; 
  } else {
    x_left = x012; 
    x_right = x02; 
    h_left = h012; 
    h_right = h02; 
  }

  // draw the horizontal segments
  for( int y = (int)p0.y ; y <=(int) p2.y; ++y ) {
    int x_l = x_left[ y - (int)p0.y];
    int x_r = x_right[ y - (int)p0.y];

    interpolate( x_l, h_left[ y -(int)p0.y], x_r, h_right[y- (int)p0.y], h );

    for( int x = x_l; x <= x_r; ++x ) {
      float l = h[ x - x_l ];
      int r = RGB5_GET_RED  (color)* l;
      if( r > 31 ) r = 31;
      int g = RGB5_GET_GREEN(color)* l;
      if( g > 31 ) g = 31;
      int b = RGB5_GET_BLUE (color)* l;
      if( b > 31 ) b = 31;      

      put_pixel( x,y, RGB5(r,g,b) );
    }
  }

}












