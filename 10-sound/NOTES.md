* create audio/ folder for .wav
* set MUSIC to audio/ in Makefile
* `make` creates the audio related files in build/

in code
1. include maxmod and the sound headers

```c
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

```
2. setup interrupts

```c
    // Maxmod requires the vblank interrupt to reset sound DMA.
    // Link the VBlank interrupt to mmVBlank, and enable it.
    irqSet( IRQ_VBLANK, mmVBlank );

```

3. init soundbani
```c
    // initialise maxmod with soundbank and 8 channels
    mmInitDefault( (mm_addr)soundbank_bin, 8 );

```

you can play sounds with a call to their ID `mmEffect(SFX_EXPLOSION);`

or setup a struct with details to alter playback a bit

```c
    // https://maxmod.org/ref/functions/mm_sound_effect.html
    mm_sound_effect pewpew = {
        { SFX_LASER},  // ID of sample to be palyed
        1024, // 6.10 fixed point (1024 = original sound?)
        0,     // handle of previous sound effect, if valid handle is given it will be recycled?
        255,     // volume 0-255 (255 loudest)
        128,     // panning.  0 far left, 255 far right
         
    };
    mmEffectEx( & pewpew );    

```


4. call `mmFrame()` in your game loop to update audio 

```c
while(true0 {
        VBlankIntrWait();
        mmFrame();

```

