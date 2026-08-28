
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
*/
    REG_TM2CNT_H = 0;   // stop timer (bit 7 really, but I'm clearing everything )
    REG_TM2CNT_L = 0;   // reset counter

    // use bit7 (TIMER_START) to start the timer
    // use bits 0&1 to set division 16.78 MHz / 1024 ~= 16384 Hz  (TIMER_DIV_1024 == 3 )
    REG_TM2CNT_H = TIMER_START | TIMER_DIV_1024;

        

	while (1) {
/*
4000100h - TM0CNT_L - Timer 0 Counter/Reload (R/W)
4000104h - TM1CNT_L - Timer 1 Counter/Reload (R/W)
4000108h - TM2CNT_L - Timer 2 Counter/Reload (R/W)
400010Ch - TM3CNT_L - Timer 3 Counter/Reload (R/W)
Writing to these registers initializes the <reload> value (but does not directly affect the current counter value). Reading returns the current <counter> value (or the recent/frozen counter value if the timer has been stopped).
*/
        uint16_t start_time = REG_TM2CNT_L;   // lower 16 is current timer val

        VBlankIntrWait();
        uint16_t end_time = REG_TM2CNT_L;
        uint16_t elapsed = end_time - start_time;

        iprintf("\x1b[2J");
        iprintf("start: %u end: %u\n", start_time, end_time );
        iprintf("Ticks/frame: %u\n", elapsed);
	}
    


}


