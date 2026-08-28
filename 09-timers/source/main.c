
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

#include <gba_timers.h>

// #define TIMER_ENABLE (1<<7) already in header as TIMER_START(BIT(7))
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
    REG_TM2CNT_H = 0;   // stop timer
    REG_TM2CNT_L = 0;   // reset counter

    // 16.78 MHz / 1024 ~= 16384 Hz
    REG_TM2CNT_H = TIMER_START | TIMER_DIV_1024;

        

	while (1) {
        uint16_t start_time = REG_TM2CNT_L;   // lower 16 is current timer val

        VBlankIntrWait();
        uint16_t end_time = REG_TM2CNT_L;
        uint16_t elapsed = end_time - start_time;

        iprintf("\x1b[2J");
        iprintf("start: %u end: %u\n", start_time, end_time );
        iprintf("Ticks/frame: %u\n", elapsed);
	}
    


}


