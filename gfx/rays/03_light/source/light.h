#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "my_math.h"

#define RGB5_GET_RED(color)   ((color) & 0x1F)
#define RGB5_GET_GREEN(color) (((color) >> 5) & 0x1F)
#define RGB5_GET_BLUE(color)  (((color) >> 10) & 0x1F)


typedef enum {
  AMBIENT,
  POINT,
  DIRECTIONAL,
} LightType;

typedef struct {
  LightType type;
  // assuming white color 
  // uint16_t color;
  
  float intensity;
  Vec3 position;
  Vec3 direction;
} Light;



#endif // _LIGHT_H_
      
