# Scrolling
Scrolling is trivially easy. Just set values for the BG of your choice

```c
REG_BG1HOFS = camera_x;
REG_BG1VOFS = camera_y;
```
It even handle screen wrap so a scrolling example is pretty trivial.

To make this a little more useful, I'm adding camera that stop at edges. 
The main things that matters are keeping track of an overall camera position for 
scrolling the map and a player (ship) position relative to the playfield size. 




# grit
The python scripts I used to break down images were just made to force myself to 
really link about the layout of the palette/tiles/maps. Going forward use the
tool made for this: `grit`

```bash
grit my_image.png -gB4 -mR4 -mLs -pn16 -ftc
```
creates two C files `my_image.h` and `my_image.c` 

* -gB4   4-bits (16 color) tiles
* -mR4   Look for duplicate/flipped tiles  palette handling 
* -mLs   GBA screen-block format
* -pn16  16 color palette
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
```txt
+----------------+----------------+
| screen block 0 | screen block 1 |
|    256x256     |    256x256     |
+----------------+----------------+
| screen block 2 | screen block 3 |
|    256x256     |    256x256     |
+----------------+----------------+
```
and uses sizes defined in `gba_video.h`

`REG_BG1CNT = ( BG_SIZE_3 | BG_16_COLOR | TILE_BASE(0) | MAP_BASE(8) );`

```bash
$ grep BG_SIZE *
gba_video.h:#define BG_SIZE(m)		((m<<14))
gba_video.h:	BG_SIZE_0		=	BG_SIZE(0),	/*!< Map Size 256x256	*/
gba_video.h:	BG_SIZE_1		=	BG_SIZE(1),	/*!< Map Size 512x256	*/
gba_video.h:	BG_SIZE_2		=	BG_SIZE(2),	/*!< Map Size 256x512	*/
gba_video.h:	BG_SIZE_3		=	BG_SIZE(3)	/*!< Map Size 512x512	*/
gba_video.h:#define BG_WID_32 BG_SIZE_0
gba_video.h:#define BG_WID_64 BG_SIZE_1
gba_video.h:#define BG_HT_32  BG_SIZE_0
gba_video.h:#define BG_HT_64  BG_SIZE_2
gba_video.h:#define ROTBG_SIZE_16  BG_SIZE_0
gba_video.h:#define ROTBG_SIZE_32  BG_SIZE_1
gba_video.h:#define ROTBG_SIZE_64  BG_SIZE_2
gba_video.h:#define ROTBG_SIZE_128 BG_SIZE_3
gba_video.h:#define TEXTBG_SIZE_256x256    BG_SIZE_0
gba_video.h:#define TEXTBG_SIZE_512x256    BG_SIZE_1
gba_video.h:#define TEXTBG_SIZE_256x512    BG_SIZE_2
gba_video.h:#define TEXTBG_SIZE_512x512    BG_SIZE_3
gba_video.h:#define ROTBG_SIZE_128x128    BG_SIZE_0
gba_video.h:#define ROTBG_SIZE_256x256    BG_SIZE_1
gba_video.h:#define ROTBG_SIZE_512x512    BG_SIZE_2
gba_video.h:#define ROTBG_SIZE_1024x1024  BG_SIZE_3
```

