# Sprites

Before getting to sprites, it's important to read Tonc's [video introduction](https://gbadev.net/tonc/video.html) 
and [sprite and tile background overview](https://gbadev.net/tonc/objbg.html) for some background.

## Palettes
The GBA can display 16-bit colors in 5.5.5 format (so 15 really) and is arranged as BGR `xbbbbbgggggrrrrr`.  
* The GBA has two 256-entry palettes. One for sprites and one for backgrounds.
  * The background palette starts at 0x05000000 ( see `gba_video.h` : `#define BG_PALETTE   ((u16 *)0x05000000) `
  * The sprite palette starts at 0x05000200 ( see `gba_video.h` : `#define SPRITE_PALETTE  ((u16 *)0x05000200)     // Sprite color table`
  * palettes can be treated as a large 256 color palet or as 16 differnt palette banks of 16 colors each.

  * Like the Mega Drive, pixels with a value of 0 are transparent.



## Tiled backgrounds
Like Atari 8-tib and C64 character modes and Sega Genesis planes, GBA has tiled backgrounds.
Instead of setting each pixel in a bitmap, you set tile ids to entries in a matrix of tiles.
I'll get to these in a later example. For now I want to learn more about sprites.

## Sprites 
GBA Sprite are range from  8x8 to 64x64 pixels in size. They can be used with either bitmap or tiled
backgrounds.  

You have 128 sprites, and like other retro-paltforms youc an move them independently of each
other and are made out of tiles.


A quick look at `gba_sprite.h` shows the possiblye size combinations
```c

enum SPRITE_SIZECODE {
                Sprite_8x8,             // OBJ_SHAPE(0) OBJ_SIZE(0)
                Sprite_16x16,   // OBJ_SHAPE(0) OBJ_SIZE(1)
                Sprite_32x32,   // OBJ_SHAPE(0) OBJ_SIZE(2)
                Sprite_64x64,   // OBJ_SHAPE(0) OBJ_SIZE(3)
                Sprite_16x8,    // OBJ_SHAPE(1) OBJ_SIZE(0)
                Sprite_32x8,    // OBJ_SHAPE(1) OBJ_SIZE(1)
                Sprite_32x16,   // OBJ_SHAPE(1) OBJ_SIZE(2)
                Sprite_64x32,   // OBJ_SHAPE(1) OBJ_SIZE(3)
                Sprite_8x16,    // OBJ_SHAPE(2) OBJ_SIZE(0)
                Sprite_8x32,    // OBJ_SHAPE(2) OBJ_SIZE(1)
                Sprite_16x32,   // OBJ_SHAPE(2) OBJ_SIZE(2)
                Sprite_32x64    // OBJ_SHAPE(2) OBJ_SIZE(3)
};
```

GBA sprites are more capable than some of the other platforms I've used. The hardware can 
flip, alpha-blend, and do some affine transformations on sprites.

From what I'm seeing in  Akkera102's [7th tutorial](https://akkera102.sakura.ne.jp/gbadev/?tutorial.7)
tile/sprite data location depend on the current graphics mode. In bitmap mode, tile data starts at
0x6014000. In a tile mode, it starts at	0x06010000. 






