# Perspective Projection
## Basics
Use the same conventions as Ray Tracking 
camera position: O = (0,0,0)
* Assuming fixed camera orientation:
  * Y+ is UP
  * Z+ is into the scene
  * X+ is to the right

* a `Viewport` is used to look at the scene. It
  * has dimensions `V_w` and `V_h`
  * is distance `d` away from O
  * centered on Z+ and perpendicular to XY

*IMP* instead of shooting a ray out from the camera into each coordinate of the 
viewport, we get a point P somewhere in front of the camera. Shoot array from point P
to O to find P' on the Viewport.

```txt
    ^Y+            - * P
    |           --   |
    |        --      |
    |     -*         |
    |  --  | P'      |
 O  |--    |         |
-----------------------------> Z+
    |<-d-> A         B
    |
    |

```
* Triangls OP'A and OPB are similar, so the proportions of their sides are the same.
* `P'_z` is d away from O because we defined the Viewport as being `d` away from O

`P'_z = d`

* ultimately we can find 

`P'_y = ( P_y * d ) / P_z`

* similarly
`P'_x = ( P_x * d ) / P_z`

we don't need `P'_z` as all points are on the viewport plane.  we do want to 
convert `P'_y` and `P'_z` to canvas coordinates

`C_x = (P'_x * C_w) / V_w`
`C_y = (P'_y * C_h) / V_h`

