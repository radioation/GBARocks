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

* a `Videport` is used to look at the scene. It
  * has dimensions `V_w` and `V_h`
  * is distance `d` away from O 
  * centered on Z+ and perpendicular to XY

## Canvas to Viewport
The chosen orientation of the viewport makes it easy to get viewport coordinates
from the canvas coordinates (a simple scale factor)`

` V_x = CANVAS_x * V_w/CANVAS_WIDTH` or  ` V_x = C_x * V_w/C_w`
` V_y = CANVAS_y * V_h/CANVAS_HEIGHT` or ` V_y = C_y * V_h/C_h`



