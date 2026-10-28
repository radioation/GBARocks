#ifndef _RAYS_H_
#define _RAYS_H_

#include "my_math.h"
#include "canvas.h"

#define VIEW_WIDTH  1
#define VIEW_HEIGHT 1
#define VIEW_DISTANCE 1

typedef struct {
	Vec3 center;
	float radius;
	int16_t color;
} sphere;


Point2 canvas_to_viewport( Point2 c );


#endif // _RAYS_H_
