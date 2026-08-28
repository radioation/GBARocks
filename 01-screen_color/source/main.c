
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

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


	// 
	SetMode( MODE_3 | BG2_ON );

	// get start of video ram
	volatile u16 *vram = (volatile u16 *)VRAM;

	// set every pixel to red
	for( int i=0; i < 240*160; ++i ) {
		vram[i] = RGB5( 31, 0, 0 );  
	}

		
	while (1) {
		VBlankIntrWait();
	}
}


