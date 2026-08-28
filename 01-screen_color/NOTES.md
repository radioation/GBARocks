# Hello World
THis is not much of a hello world, but AFAIK libgba doesn't bother with fonts, so a simple 
fill of VRAM with red seems like a decent first step. libGBA seems pretty undocumented,
 but the headers (at least so far) look usable.


I've been looking over Kyle Halladay's [GBA By Example](https://kylehalladay.com/blog/tutorial/gba/2017/03/28/GBA-By-Example-1.html)

```c
typedef unsigned short     uint16;
typedef unsigned int       uint32;

#define REG_DISPLAYCONTROL *((volatile uint32*)(0x04000000))
#define VIDEOMODE_3         0x0003
#define BGMODE_2            0x0400

#define SCREENBUFFER        ((volatile uint16*)0x06000000)
#define SCREEN_W            240
#define SCREEN_H            160

int main()
{
    REG_DISPLAYCONTROL = VIDEOMODE_3 | BGMODE_2;

    for (int i = 0; i < SCREEN_W * SCREEN_H; ++i)
    {
    	SCREENBUFFER[i] = 0xFFFF;
    }

    while(1){}
    return 0;
}   
```

I've also been looking through Akkera102's [GBA posts](https://akkera102.sakura.ne.jp/gbadev/index.php?tutorial.2)
and they mention `SetMode( MODE_3 | BG2_ENABLE )`

Poking around a bit in the headers yields:
* `SetMode()` in `gba_video.h`. This is just an inline function to set `REG_DISPCNT`
```c
static inline void SetMode(int mode)    {REG_DISPCNT = mode;}
M
```
where `REG_DISPCNT` is  `#define REG_DISPCNT             *((vu16 *)(REG_BASE + 0x00))`
and `REG_BASE` in `gba_base.h` is  `#define REG_BASE        0x04000000`


So `SetMode( MODE_3 | BG2_ON )` is equivalent to 
`REG_DISPCNT = MODE_3 | BG2_ON`

* `VRAM` is also in `gba_base.h` and is defined as `#define VRAM            0x06000000`


* `RGB5()` in `gba_video.h` is a macro to set the color of a pixel. 
  It's defined as `#define RGB5(r,g,b) ((r)|((g)<<5)|((b)<<10))`


## VBLank

Unlike Halladay's example, the libgba template has a set of calls for vblank interupts
```c
irqInit();
irqEnable(IRQ_VBLANK);

...

while(1) {
   VBLankIntrWait();
}
```
I doubt I need to wait for the VBlank in this test program (nothing to cause screen tearing 
in a solid red screen), but I'm leaving them in.


