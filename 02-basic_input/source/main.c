
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>


static inline void draw_square_at( u32 x, u32 y, u16 color ) {

        for( int j=0; j < 10; ++j) {
                for( int i=0; i < 10; ++i) {
                        ((volatile u16 *)VRAM)[ (x + i ) + ( y + j ) * 240 ] = color;
                }
        }
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


	while (1) {
		VBlankIntrWait();
		scanKeys(); // Call this function once per main loop in order to use the keypad functions.
			    
	
		u16 down = keysDown();		

		if( down & KEY_UP ) {
			draw_square_at( 30, 30, RGB5( 31,31,31) );
		}
		if( down & KEY_DOWN ) {
			draw_square_at( 30, 70, RGB5( 31,31,31) );
		}
		if( down & KEY_LEFT ) {
			draw_square_at( 10, 50, RGB5( 31,31,31) );
		}
		if( down & KEY_RIGHT ) {
			draw_square_at( 50, 50, RGB5( 31,31,31) );
		}
		if( down & KEY_R ) { // shoulder button
			draw_square_at( 229, 0, RGB5( 31,31,31) );
		}
		if( down & KEY_L ) { // shoulder button
			draw_square_at( 0, 0, RGB5( 31,31,31) );
		}
		if( down & KEY_A ) {
			draw_square_at( 200, 70, RGB5( 31,31,31) );
		}
		if( down & KEY_B ) {
			draw_square_at( 170, 70, RGB5( 31,31,31) );
		}

		u16 up = keysUp();		

		if( up & KEY_UP ) {
			draw_square_at( 30, 30, RGB5( 0,0,4) );
		}
		if( up & KEY_DOWN ) {
			draw_square_at( 30, 70, RGB5( 0,0,4) );
		}
		if( up & KEY_LEFT ) {
			draw_square_at( 10, 50, RGB5( 0,0,4) );
		}
		if( up & KEY_RIGHT ) {
			draw_square_at( 50, 50, RGB5( 0,0,4) );
		}
		if( up & KEY_R ) { // shoulder button
			draw_square_at( 229, 0, RGB5( 0,0,4) );
		}
		if( up & KEY_L ) { // shoulder button
			draw_square_at( 0, 0, RGB5( 0,0,4) );
		}
		if( up & KEY_A ) {
			draw_square_at( 200, 70, RGB5( 0,0,4) );
		}
		if( up & KEY_B ) {
			draw_square_at( 170, 70, RGB5( 0,0,4) );
		}

	}
}


