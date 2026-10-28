#include "rays.h"
#include "canvas.h"




Point2 canvas_to_viewport( Point2 c ) {
	Point2 v;
	v.x = c.x * VIEW_WIDTH / CANVAS_WIDTH;
	v.y = c.y * VIEW_HEIGHT / CANVAS_HEIGHT;
	return v;
}


