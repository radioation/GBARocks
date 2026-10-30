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


## Scenes
Important points that I mostly remembered on a high level (but forgot the matrix algebra details of)

### Object representation
* Use triangles.  Any other polygon, can be decomposed into triangles.
  (not mentioned, but triangles will always be planar, and Squares might not be)
* Triangles exist of Verticies, but simply listing verticies isn't enough. We
  need to define triangles as sets of 3 vertices to make up the side. 

*IMP* This suggests a generic structure to represent any object made of triangles:
  1. A `Verticies` list holding the coordinates of each vertex (in MODEL space)
  2. A `Triangles` list specifying by index which sets of three vertices describe
    triangles on the surface of the object.  (He goes on to add colors to the triangls )

 *IIRC* there may be some advantage to speifing the vertices for a triangle in a consistent 
 order to define normals pointing outward from the object. (not mentioned here)(

*IMP* Object REndering: FIRST project every vertex and store in in a vertices 
list (this way we don't repeat the calcualtions for shared verticies) SECOND go
through the triangles list and render reach 


*IMP* The object triangles are defined in `MODEL SPACE` and are likely defined all around 
the camera O. So we would need to at least Translate the object to be in front of the camera
`V' = V + T` where V is a vertex, T is a translation vector that moves it in front of camera, 
V' is the translated vertex. IN this case the translated vertices would be in `WORLD SPACE`

### Models and Instances
You often want to render multiple copies of an object in a game (players, NPCs, environment 
objects). Objects can have a *lot* of verticies and having a copy of the model for 
each would waste a lot of memory. INstead of having copies use Models and INstances
* Model: a set of verticies and triangles describing a certain object in a generic way
* Instance: A concrete occurance of that model within the scene.  In code, the 
  instance would have a reference to the original model and some instance-specific
  data like position, orientation, scaling, coloring etc for rendering.

so basicaly
1. RenderScene() would iterate over instances
2. for each instance the RenderInstance(i) would be called
3. RenderInstance() would get the model for the instance.
  a. it'd iterate over the model vertices, project them, and 
     store them for triangle rendering
  b. it'd iterate over the modle triangles and render 
     triangles based on the projected vertices from 3a

*IMP* Translation is not the only thing that we do to models. We want a 
 `MODEL TRANSFORM` that has at least a scaling factor, a rotation around the origin
  *in model space*, and a translation 
*IMP* Translation must be done last, because you usually want to rotate and 
 scale around the ORIGIN in model space

 


