
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>


//---------------------------------------------------------------------------------
static inline void set_pixel( u32 x, u32 y, u16 color ) {
	((u16 *)VRAM)[ x + y * 240 ] = color;
}

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


	SetMode( MODE_3 | BG2_ON );

	int x = 0;
	int y = 0;
	while (1) {
		VBlankIntrWait();
		set_pixel( x, y, RGB5( 0, 0, 0 ));
		x++; if( x >= 240 ) x = 0;
		y++; if( y >= 160 ) y = 0;
		set_pixel( x, y, RGB5( 31, 0, 0 ));

	}
}


