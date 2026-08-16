
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_dma.h>
#include <gba_sprites.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myfix.h"

#include "space.h"
#include "ship.h"

extern const  u16 map[1024];
#define OAM_MEM ((volatile OBJATTR *)0x07000000)
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
	REG_DISPCNT = ( MODE_0|BG1_ON | OBJ_ENABLE | OBJ_1D_MAP );


	//memcpy( BG_PALETTE, spacePal, spacePalLen );
	dmaCopy( spacePal, BG_PALETTE, spacePalLen );

	// copy tile data into VRAM address for tile base 0
	u16* bg_tile_vram = (u16*)TILE_BASE_ADR( 0 );
	//memcpy( bg_tile_vram, spaceTiles, spaceTilesLen ); NO WORKY!
	dmaCopy( spaceTiles, bg_tile_vram, spaceTilesLen );
	//u16* map_vram = (u16*)MAP_BASE_ADR( 8 );
	dmaCopy( spaceMap, MAP_BASE_ADR(8), spaceMapLen );



	// sprites
	SPRITE_PALETTE[0] = RGB8( 255, 0, 0 );
	SPRITE_PALETTE[1] = RGB8( 255, 255, 255 );
	SPRITE_PALETTE[2] = RGB8( 218, 218, 218 );
	SPRITE_PALETTE[3] = RGB8( 182, 182, 182 );
	SPRITE_PALETTE[4] = RGB8( 145, 145, 145 );
	SPRITE_PALETTE[5] = RGB8( 109, 109, 109 );
	SPRITE_PALETTE[6] = RGB8( 72, 72, 72 );
	SPRITE_PALETTE[7] = RGB8( 36, 36, 36 );
	SPRITE_PALETTE[8] = RGB8( 0, 0, 0 );
	SPRITE_PALETTE[9] = RGB8( 0, 109, 218 );
	SPRITE_PALETTE[10] = RGB8( 0, 72, 109 );
	SPRITE_PALETTE[11] = RGB8( 0, 36, 72 );
	SPRITE_PALETTE[12] = RGB8( 0, 36, 145 );
	SPRITE_PALETTE[13] = RGB8( 0, 36, 109 );
	SPRITE_PALETTE[14] = RGB8( 145, 36, 0 );
	SPRITE_PALETTE[15] = RGB8( 255, 145, 0 );

	for(int i = 0; i < 128; i++) {
		OAM_MEM[i].attr0 = ATTR0_DISABLED;
	}

	u16* sprite_vram = (u16*)( 0x06010000); // CHARACTER/TILED MODE SO 
	dmaCopy( shipTiles, sprite_vram, shipTilesLen );
	OAM_MEM[0].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(40); 
	OAM_MEM[0].attr1 = ATTR1_SIZE_16 | OBJ_X(50);
	OAM_MEM[0].attr2 = OBJ_CHAR(0) | OBJ_PRIORITY(0);


	// use background 0 with tile base 0
	//    #define TEXTBG_SIZE_256x256    BG_SIZE_0
	//    BG_16_COLOR             =       (0<<7),         /*!< background uses 16 color tiles             */
	REG_BG1CNT = ( BG_SIZE_3 | BG_16_COLOR | TILE_BASE(0) | MAP_BASE(8) );

	int camera_x = 0;
	int camera_y = 0;
	int tick = 0;
	int shipFrame = 0;
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

		tick++;
		if( tick % 5 == 0 ) {	
			shipFrame+=4;
			if( shipFrame > 28 ) shipFrame = 0;
			OAM_MEM[0].attr2 &= 0xfc00;
			OAM_MEM[0].attr2 |= OBJ_CHAR(shipFrame);
		}


		REG_BG1HOFS = camera_x;
		REG_BG1VOFS = camera_y;
	}
}


