import math
print('#include "myfix.h"')

print("const myfix sinTab[256] = {")
for step in range( 0, 256 ):
    rad = step * (360.0/256.0) * math.pi  / 180.0
    print ("  MYFIX( %f ),   // step: %d rad: %f deg: %f" % ( math.sin( rad ),  step, rad , rad * 57.2958) )
print("};")

print("const myfix cosTab[256] = {")
for step in range( 0, 256 ):
    rad = step * (360.0/256.0) * math.pi  / 180.0
    print ("  MYFIX( %f ),   // step: %d rad: %f deg: %f" % ( math.cos( rad ),  step, rad , rad * 57.2958) )
print("};")



THRUST_DIV = 7.0
MAX_SPEED = 3.0

print("const myfix thrustX[256] = {")
for step in range( 0, 256 ):
    rad = step * (360.0/256.0) * math.pi  / 180.0
    print ("  MYFIX( %f ),   // step: %d rad: %f deg: %f" % ( math.cos( rad )/THRUST_DIV,  step, rad , rad * 57.2958) )
print("};\n")

print("const myfix maxSpeedX[256] = {")
for step in range( 0, 256 ):
    rad = step * (360.0/256.0) * math.pi  / 180.0
    print ("  MYFIX( %f ),   // step: %d rad: %f deg: %f" % ( math.cos( rad )* MAX_SPEED,  step, rad , rad * 57.2958) )
print("};\n")


print("const myfix thrustY[256] = {")
for step in range( 0, 256 ):
    rad = step * (360.0/256.0) * math.pi  / 180.0
    print ("  MYFIX( %f ),   // step: %d rad: %f deg: %f" % (math.sin( rad )/THRUST_DIV,  step, rad , rad * 57.2958) )
print("};\n")


print("const myfix maxSpeedY[256] = {")
for step in range( 0, 256 ):
    rad = step * (360.0/256.0) * math.pi  / 180.0
    print ("  MYFIX( %f ),   // step: %d rad: %f deg: %f" % (math.sin( rad )* MAX_SPEED,  step, rad , rad * 57.2958) )

print("};\n")





