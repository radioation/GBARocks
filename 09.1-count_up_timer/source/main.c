
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

#include <gba_timers.h>

// #define TIMER_ENABLE (1<<7) already in header as TIMER_START(BIT(7))

// prescaler values
#define TIMER_DIV_1     0
#define TIMER_DIV_64    1
#define TIMER_DIV_256   2
#define TIMER_DIV_1024  3


//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
	//---------------------------------------------------------------------------------


	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);

    consoleDemoInit();

    //////////////////////////////////////////////////////////////
    // INIT TIMER 2 with division by 1024

/*
4000102h - TM0CNT_H - Timer 0 Control (R/W)
4000106h - TM1CNT_H - Timer 1 Control (R/W)
400010Ah - TM2CNT_H - Timer 2 Control (R/W)
400010Eh - TM3CNT_H - Timer 3 Control (R/W)
  Bit   Expl.
  0-1   Prescaler Selection (0=F/1, 1=F/64, 2=F/256, 3=F/1024)
  2     Count-up Timing   (0=Normal, 1=See below)  ;Not used in TM0CNT_H
  3-5   Not used
  6     Timer IRQ Enable  (0=Disable, 1=IRQ on Timer overflow)
  7     Timer Start/Stop  (0=Stop, 1=Operate)
  8-15  Not used

When Count-up Timing is enabled, the prescaler value is ignored, instead the
time is incremented each time when the previous counter overflows. This
function cannot be used for Timer 0 (as it is the first timer).  F = System
Clock (16.78MHz).
*/
    REG_TM2CNT_H = 0;   // stop timer (bit 7 really, but I'm clearing everything )
    REG_TM3CNT_H = 0;  
/*
4000100h - TM0CNT_L - Timer 0 Counter/Reload (R/W)
4000104h - TM1CNT_L - Timer 1 Counter/Reload (R/W)
4000108h - TM2CNT_L - Timer 2 Counter/Reload (R/W)
400010Ch - TM3CNT_L - Timer 3 Counter/Reload (R/W)
Writing to these registers initializes the <reload> value (but does not directly affect the current counter value). Reading returns the current <counter> value (or the recent/frozen counter value if the timer has been stopped).
*/

    // we want timer 2 to take appx 1 second to overflow. Using 1024 divisor
    //  65536 - 16384 = 49152 -> 49152 == C000
    REG_TM2CNT_L = 0xC000;   // change start value for timer 2 to 49152 
    REG_TM3CNT_L = 0;        // set start value for timer 3 to 0.. will increment when timer 2 overflows (appx every second)

    // use bit7 (TIMER_START) to start the timer
    // use bits 0&1 to set division 16.777216 MHz / 1024 ~= 16384 ticks/sec (TIMER_DIV_1024 == 3 )
    REG_TM2CNT_H = TIMER_START | TIMER_DIV_1024;
    // use bit2 (TIMER_COUNT) uses preceding timer (2) to tell timer 3 to tick up
    REG_TM3CNT_H = TIMER_START | TIMER_COUNT; 

    // if we're overflowing 
        

	while (1) {

        VBlankIntrWait();
        uint16_t curr_time_s = REG_TM3CNT_L;

        iprintf("\x1b[2J");
        iprintf("seconds: %u\n", curr_time_s );
	}
    


}


