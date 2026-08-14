# Putting Pixels
One thing I really like compared to the Sega Genesis / MegaDrive is having 
actual bitmaps to work with. Tiles are great, but not being able to draw
to individual pixels complicates things that I could do easily on a system
like an Atari or C64.

As pointed out by [tonc's tutorial](https://gbadev.net/tonc/first.html), 
Mode 3 VRAM is a 16-bit bitmap. It's 240 x 160 with every 240 pixels representing
a row in the display. So we can set individual pixels with something like 

```c
static inline void set_pixel( u32 x, u32 y, u16 color ) {
        ((u16 *)VRAM)[ x + y * 240 ] = color;
}
```

_NOTE_ I did notice that calling `set_pixel()` immediatelyi after `VBlankIntrWait()` like this:
```c
		VBlankIntrWait();
		set_pixel( x, y, RGB5( 0, 0, 0 ));
		x++; if( x >= 240 ) x = 0;
		y++; if( y >= 160 ) y = 0;
		set_pixel( x, y, RGB5( 31, 0, 0 ));
```
occasionally leaves red pixels at seemingly random positions on the display in mGBA. Adding 
a small delay after the wait seems to mostly fix fix this, but I'd rather not waste cycles
in an empty loop.  So I've updated code to store old and new x y values.

