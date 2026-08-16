
# grit
The python scripts I used to break down images were just made to force myself to 
really link about the layout of the palette/tiles/maps. Going forward use `grit`

```bash
grit my_image.png -gB4 -mR4 -mLs -pn16 -ftc
```
creates two C files `my_image.h` and `my_image.c` 

* -gB4   4-bits (16 color) tiles
* -mR4   Look for duplicate/flipped tiles  palette handling 
* -mLs   GBA screen-block format
* -ftc   create C arrays/header

TO get this to work include the files created
```c
#include "my_image.h"
```

and copy using `dmaCopy`. `memcpy()` may not work.

```c
dmaCopy( my_imagePal, BG_PALETTE, my_imagePalLen );  
dmaCopy( my_imageTiles, bg_tile_vram, my_imageTilesLen );
dmaCopy( my_imageMap, MAP_BASE_ADR(8), my_imageMapLen );
```


# bigger maps
I'm going with 480x480 which will still fit into a 512x512 tile map

+----------------+----------------+
| screen block 0 | screen block 1 |
|    256x256     |    256x256     |
+----------------+----------------+
| screen block 2 | screen block 3 |
|    256x256     |    256x256     |
+----------------+----------------+


