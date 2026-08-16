
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_dma.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "space.h"
extern const  u16 map[1024];

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

	//SetMode( MODE_0 | BG1_ON );  // 4 backgoroudns, using BG2
        REG_DISPCNT = ( MODE_0|BG1_ON );

	
	//memcpy( BG_PALETTE, spacePal, spacePalLen );
	dmaCopy( spacePal, BG_PALETTE, spacePalLen );

	// copy tile data into VRAM address for tile base 0
	u16* bg_tile_vram = (u16*)TILE_BASE_ADR( 0 );
	//memcpy( bg_tile_vram, spaceTiles, spaceTilesLen ); NO WORKY!
	dmaCopy( spaceTiles, bg_tile_vram, spaceTilesLen );
	//u16* map_vram = (u16*)MAP_BASE_ADR( 8 );
	dmaCopy( spaceMap, MAP_BASE_ADR(8), spaceMapLen );




	// use background 0 with tile base 0
	//    #define TEXTBG_SIZE_256x256    BG_SIZE_0
	//    BG_16_COLOR             =       (0<<7),         /*!< background uses 16 color tiles             */

	REG_BG1CNT = ( BG_SIZE_3 | BG_16_COLOR | TILE_BASE(0) | MAP_BASE(8) );

	int camera_x = 0;
	int camera_y = 0;
	while (1) {
		VBlankIntrWait();
		scanKeys();
		u16 keys = keysHeld();

		if (keys & KEY_LEFT)
			camera_x--;

		if (keys & KEY_RIGHT)
			camera_x++;

		if (keys & KEY_UP)
			camera_y--;

		if (keys & KEY_DOWN)
			camera_y++;

		REG_BG1HOFS = camera_x;
		REG_BG1VOFS = camera_y;
	}
}


