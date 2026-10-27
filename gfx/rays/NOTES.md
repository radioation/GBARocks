# About.
THese are just some code written while reading 'Computer Graphics from Scratch' 
by Gabriel Gambetta. 

This is a learning exercise and I'm largely not bothering with efficiency.

## coordinates

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

