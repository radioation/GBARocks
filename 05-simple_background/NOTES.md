# TL;DR

CHARACTER/TILE backgrounds need

1. Palette: can copy your custom palettes to `BG_PALETTE`
2. Tile data in vram. can copy to `TITLE_BASE_ADR( t )`
3. MAP data in vram. can copy to `MAP_BASE_ADR( m )`
4. tell control register where
`REG_BG2CNT = ( BG_SIZE_0 | BG_16_COLOR | TILE_BASE(t) | MAP_BASE(m) );`


#  TExt Backgrounds
* Text backgrounds are tile-based like the C64/A8 text modes.

* 3 modes 
  * mode  0 has 4 backgrounds BG0-BG3
  * mode 1 has two backgrounds BG0, BG1 and one affine BG2
  * mode 2 has two affine BG2 and BG3

* Address of the data is set using  `REG_BG0CNT` to `REG_BG3CNT`
  where `REG_BASE    0x04000000 `
```bash
:~/libgba/include$ grep REG_BG[0-3]CNT *  | grep define
gba_video.h:#define REG_BG0CNT	*((vu16 *)(REG_BASE + 0x08))
gba_video.h:#define REG_BG1CNT	*((vu16 *)(REG_BASE + 0x0a))
gba_video.h:#define REG_BG2CNT	*((vu16 *)(REG_BASE + 0x0c))
gba_video.h:#define REG_BG3CNT	*((vu16 *)(REG_BASE + 0x0e))
```

`REG_BG0CNT`  0x0400 0008
`REG_BG1CNT`  0x0400 000a
`REG_BG2CNT`  0x0400 000C
`REG_BG3CNT`  0x0400 000E

Bits are (From [gbadoc](https://gbadev.net/gbadoc/registers.html#REG_BGCNT)
F E D C  B A 9 8  7 6 5 4  3 2 1 0 
Z Z V M  M M M M  A C X X  S S P P


* (P) priority - bits 0 and 1.  00 == high priority, 11 lowest.
 `#define BG_PRIORITY(m)          ((m))`

* (S) start address of tile data - bits 2and 3,  

   Tiles start at  0x0600 0000 and offset by `S*0x4000`
  `#define TILE_BASE(m)            ((m) << 2)`
  `#define TILE_BASE_ADR(m)        ((void *)(VRAM + ((m) << 14)))`

* (X) not used? must be 0?

* (C) mosaic effect ( 0= disable, 1 = enable);


* (A) bit 7.  Backgrounds can be 8-bit mode (bit 7 is 1) or 4-bit mode
    bit 7 is0 (16, 16 color palettes)

* (M) BIT8-C address of tile map  0x0600 0000 and offset by `M*0x4000`
 `define MAP_BASE(m)                 ((m) << 8)`
 `#define MAP_BASE_ADR(m)             ((void *)(VRAM + ((m) << 11)))`

  screen blocks are always 32x32 in size.  so one block is 256x256 pixels

* (D) bit D  tile or not tile rotationla backgrounds

* (Z) bits E and F
  For “text” backgrounds:
  
  00 = 256x256 (32x32 tiles) (a single block)
  01 = 512x256 (64x32 tiles)
  10 = 256x512 (32x64 tiles)
  11 = 512x512 (64x64 tiles)
```c

#define TEXTBG_SIZE_256x256    BG_SIZE_0
#define TEXTBG_SIZE_512x256    BG_SIZE_1
#define TEXTBG_SIZE_256x512    BG_SIZE_2
#define TEXTBG_SIZE_512x512    BG_SIZE_3
```


* Palettes start at  0x0500 0000
```bash
$ grep 0x0500 *
gba_video.h:#define BG_COLORS		((vu16 *)0x05000000)	// Background color table
gba_video.h:#define BG_PALETTE		((u16 *)0x05000000)	// Background color table
gba_video.h:#define	OBJ_COLORS		((u16 *)0x05000200)	// Sprite color table
gba_video.h:#define	SPRITE_PALETTE	((u16 *)0x05000200)	// Sprite color table
```



# scroll
 HOFS / VOFS registers can be used to scroll around a larger area of up to
  512x512 pixels (or 64 x 64 tiles).
```bash
e$ grep REG_BG[0-3].OFS *  | grep define
gba_video.h:#define	REG_BG0HOFS		*((vu16 *)(REG_BASE + 0x10))	// BG 0 H Offset
gba_video.h:#define	REG_BG0VOFS		*((vu16 *)(REG_BASE + 0x12))	// BG 0 V Offset
gba_video.h:#define	REG_BG1HOFS		*((vu16 *)(REG_BASE + 0x14))	// BG 1 H Offset
gba_video.h:#define	REG_BG1VOFS		*((vu16 *)(REG_BASE + 0x16))	// BG 1 V Offset
gba_video.h:#define	REG_BG2HOFS		*((vu16 *)(REG_BASE + 0x18))	// BG 2 H Offset
gba_video.h:#define	REG_BG2VOFS		*((vu16 *)(REG_BASE + 0x1a))	// BG 2 V Offset
gba_video.h:#define	REG_BG3HOFS		*((vu16 *)(REG_BASE + 0x1c))	// BG 3 H Offset
gba_video.h:#define	REG_BG3VOFS		*((vu16 *)(REG_BASE + 0x1e))	// BG 3 V Offset
```

`REG_BG0HOFS`		0x0400 0010
`REG_BG0VOFS`		0x0400 0012
`REG_BG1HOFS`		0x0400 0014
`REG_BG1VOFS`		0x0400 0016
`REG_BG2HOFS`		0x0400 0018
`REG_BG2VOFS`		0x0400 001a
`REG_BG3HOFS`		Mx0400 001c
`REG_BG3VOFS`		0x0400 001e


# generat1ee level
python3 genlvl.py  --width 15 --height 10 --scale 8 --seed 125
