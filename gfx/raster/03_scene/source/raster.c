#include "raster.h"

Point2 viewport_to_canvas( float x, float y ) {
  return (Point2) { x * (float)CANVAS_WIDTH/(float)VIEW_WIDTH, y * (float)CANVAS_HEIGHT/(float)VIEW_HEIGHT };
}


Point2 project_vertex( Vec3 v ) {
  return viewport_to_canvas( v.x * (float)VIEW_DISTANCE / v.z, v.y * (float)VIEW_DISTANCE / v.z );
}


