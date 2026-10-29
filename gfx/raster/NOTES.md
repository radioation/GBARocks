# About
More code based on reading 'Computer Graphics from Scratch' by Gabriel Gambetta.
This time the rasterization chapters. Chapters 6-8 cover things I think 
I remember pretty well.

# Lines/Triangles
## Lines
They spent two pages deriving y=mx+b from two 2D points. 
* Book uses: `y = ax + b` where a is the slope (rise over run) instead of m.
  * rise over run does run into a problem when run == 0
* The book goes over several DrawLine() functions, which I won't bother repeating.
 It's best to read the book.
  * their first naive implementation goes over every possible x (run) and 
     multiplies it by `a` and adds `b` to get a point in the line. 
     `y = a * x + b`
  * The next version recognizes that slope *is* constant and gets rid of the 
    multiplication with x (expensive) and just increases y by a.
```pseudo
    y = y0
    for x = x0 to x 
       put_pixel( x,y, color)
       y = y + a
```
  * the case where x0 is > x1 is handled by swapping the values of x0 and x1

*IMP* It should be noted, a might be > 1. Steeper slopes *will* cause gaps
 in the y direction. A possible solution is to switch drawing routines
  based on the slope. It's just the above but iterates y0 to y and increments
  x.

*IMP* The two cases are basically the same and can be abstracted into a general
 interpolation function.
 The interpolate function returns an array of dependent float/real for each
 x (or y) integer (pixel location).


## Wireframe Triangles
This is easy, just call DrawLines for each side. Nothing to learn here.

## Filled Triangles
Rather than draw the three sides of the triangle, filled triangles can draw 
the horizontal segments from one side to another. The basic algo is

```pseudo
for each horizontal line y between the triangle top and bottom:
    find x_left and x_right for current y
    draw_line( x_left, x_right, color )
```
To the triangle top-to-bottom, they sort the 3 points on y

```pseudo
if y1 < y0 { swap(p1, p0) }
if y2 < y0 { swap(p2, p0) }
if y2 < y1 { swap(p2, p1) }
```
*IMP* This simplifies the function as we can always assume p0 is the lowest
  point of the triangle and p2 is the highest. p0 to p2 will be the Tall side
  of the triangle.
*IMP* interpolate is used to get the X correspobding to the y's of each 
 line segment. We'll either use the Tall Side to find the right X values
 or combine the two shorter to find the right X values. Left X will
 come from the other.
```pseudo
// combined sides
x01[] = interpolate( y0, x0, y1, x1 )
x12[] = interpolate( y1, x1, y2, x2 )
// Tall side
x02[] = interpolate( y0, x0, y2, x2 )
```
*IMP* note that x01[] + x12[] will have one line of overlap.
   they removethe last item from it and create 
   `x012[] = x0[:-1] + x12`


