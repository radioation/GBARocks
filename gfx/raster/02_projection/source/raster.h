#ifndef _RASTER_H_
#define _RASTER_H_
#include "my_math.h"
#include "canvas.h"


#define VIEW_WIDTH  1
#define VIEW_HEIGHT 1
#define VIEW_DISTANCE 1



Point2 viewport_to_canvas( float x, float y );
Point2 project_vertex( Vec3 v );


#endif // _RASTER_H_
