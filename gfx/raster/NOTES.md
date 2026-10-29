# About
More code based on reading 'Computer Graphics from Scratch' by Gabriel Gambetta.
This time the rasterization chapters. Chapters 6-8 cover things I think 
I remember pretty well.

## Lines/Triangles
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

