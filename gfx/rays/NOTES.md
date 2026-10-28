# About.
THese are just some code written while reading 'Computer Graphics from Scratch' 
by Gabriel Gambetta. 

This is a learning exercise and I'm largely not bothering with efficiency.

# coordinates

## Canvas Coords
The book refers to a Canvas that has the same resolution as the Screen. Canvas is shifted 
to have the origin (0,0) at the center and inverts the Y-axis.

So it the coordinate system runs [ -CANVAS-WIDTH/2, CANVAS-WIDTH/2 ) left to right
and [ -CANVAS-HEIGHT/2, CANVAS-HEIGHT/2 ) Bottom to top (flipping Y)

The book relies on a putPixel function to assign colors to pixels on screen
`canvas.PutPixel(x,y,color)`

This converts to screen coordinates ( and wastes some cycles ) with
```c
    s16 screen_x = CANVAS_WIDTH/2 + canvas_x;  // translate X
    s16 screen_y = CANVAS_HEIGHT/2 - canvas_y; // translate and flip Y
```

## Scene Coords
Objects we want to render are placed in a Scene. The scene is 3D with
* Y is the UP axis
* X and Z define the 'floor'.
  * X 

# Basic Rays

* Assuming fixed camera position: O = (0,0,0)
* Assuming fixed camera orientation:
  * Y+ is UP
  * Z+ is into the scene
  * X+ is to the right

* a `Viewport` is used to look at the scene. It
  * has dimensions `V_w` and `V_h`
  * is distance `d` away from O 
  * centered on Z+ and perpendicular to XY

## Canvas to Viewport
The chosen orientation of the viewport makes it easy to get viewport coordinates
from the canvas coordinates (a simple scale factor)

` V_x = CANVAS_x * V_w/CANVAS_WIDTH` or  ` V_x = C_x * V_w/C_w`
` V_y = CANVAS_y * V_h/CANVAS_HEIGHT` or ` V_y = C_y * V_h/C_h`
` V_z = d`.

##  Catching Rays
It's all math. Without going into things too deeply, we use an 
equation to represent the current ray starting from the camera O
and passing through the Viewport V.  Any point P can be found with

`P = O +t( V- O )`  or `P=O+tD` where D is the direction defined by V-O
where t is any number. Realistically though we just need positive t,
and basically just t values past the viewport.

Spheres are easily defined as all points at distance r (radius) from a center point C

`distance(P,C) = r`

basically the length of any point from C is positive and equal to r
`|P-C| = r`
this can be rewritten as
`<P-C, P-C> = r^2`  (see the book, will do actual long-winded on paper)


we basically want to find the interseciton of the ray with the sphere. 
We don't have to plug in every possible t and match the result up with 
the sphere equation, we just use the discriminant. (again, see book)
```txt
a = <D, D>

b = w<CO, D>

c = <CO, CO> = r^2

at^2 + bt + c = 0

```
so solve for `{t1, t2} = (-b + sqrt( b^2 - 4ac) / 2a`




