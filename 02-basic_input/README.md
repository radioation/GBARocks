# Basic input
A quick search of `libgba/include` shows a single file called `gba_input.h`. 
Unsurprisingly, there's not much there:
```bash
c:~/libgba/include$ grep fn gba_input.h
#ifndef _gba_input_h_
/*! \fn scanKeys()
/*! \fn u16 keysDown()
/*! \fn u16 keysDownRepeat()
/*! \fn u16 keysUp()
/*! \fn u16 keysHeld()
/*! \fn setRepeat(int SetDelay, int SetRepeat)

```


_NOTE_: `scanKeys()` must be called in your main loop in order for the rest of the functions 
to work.

For now I'll just try out `keysDown()` and `keysUp()`

An enum is defined to handle each of the buttons and DPAD
```c
typedef enum KEYPAD_BITS {
        KEY_A           =       (1<<0), /*!< keypad A button */
        KEY_B           =       (1<<1), /*!< keypad B button */
        KEY_SELECT      =       (1<<2), /*!< keypad SELECT button */
        KEY_START       =       (1<<3), /*!< keypad START button */
        KEY_RIGHT       =       (1<<4), /*!< dpad RIGHT */
        KEY_LEFT        =       (1<<5), /*!< dpad LEFT */
        KEY_UP          =       (1<<6), /*!< dpad UP */
        KEY_DOWN        =       (1<<7), /*!< dpad DOWN */
        KEY_R           =       (1<<8), /*!< Right shoulder button */
        KEY_L           =       (1<<9), /*!< Left shoulder button */

        KEYIRQ_ENABLE   =       (1<<14),        /*!< Enable keypad interrupt */
        KEYIRQ_OR               =       (0<<15),        /*!< interrupt logical OR mode */
        KEYIRQ_AND              =       (1<<15),        /*!< interrupt logical AND mode */
        DPAD            =       (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT) /*!< mask all dpad buttons */
} KEYPAD_BITS;

```



