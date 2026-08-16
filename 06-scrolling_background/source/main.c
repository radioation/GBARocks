
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


#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 16
#define PLAYFIELD_WIDTH 480
#define PLAYFIELD_HEIGHT 480
#define SCR_WIDTH 240
#define SCR_HEIGHT 160
#define CAMERA_PADDING 40

int camPosX =0;
int camPosY = 0;
int shipPosX = 120;
int shipPosY = 80;

bool updatePlayerPosition = false;

static void updateCameraPos() {
	// figure out where the ship is.  
	int shipScreenX = shipPosX - camPosX;
	int shipScreenY = shipPosY - camPosY;

	// Adjust new camera X position based on ship position
	int newCamX;
	// check if the ship X position is too close to the right edge of the screen
	if( shipScreenX > SCR_WIDTH - CAMERA_PADDING - PLAYER_WIDTH ) {
		newCamX = shipPosX - ( SCR_WIDTH - CAMERA_PADDING - PLAYER_WIDTH );
	} else if( shipScreenX < CAMERA_PADDING ) { // check if the ship is too close to the left
		newCamX = shipPosX - CAMERA_PADDING;	
	} else {
		newCamX = camPosX; // no change to camera position.
	}

	// Adjust camera Y position based on ship position
	int newCamY;
	// check if the ship Y position is too close to the bottom edge of the screen
	if( shipScreenY > SCR_HEIGHT - CAMERA_PADDING - PLAYER_HEIGHT ) {
		newCamY = shipPosY - ( SCR_HEIGHT - CAMERA_PADDING - PLAYER_HEIGHT ) ;
	} else if( shipScreenY < CAMERA_PADDING ) {  // is ship too close to the top of the screen?
		newCamY = shipPosY - CAMERA_PADDING;	
	} else {
		newCamY = camPosY; // no change to camera position.
	}


	// handle camera position at edges
	if ( newCamX < 0  ) { // don't move past the left edge of the scroll image.
		newCamX = 0;
	} else if ( newCamX > (PLAYFIELD_WIDTH - SCR_WIDTH )) {  // don't move past the right edge
		newCamX = PLAYFIELD_WIDTH - SCR_WIDTH ;
	}
	if ( newCamY < 0  ) { // don't move past the top of the scroll image
		newCamY = 0;
	} else if ( newCamY > (PLAYFIELD_HEIGHT - SCR_HEIGHT )) {  // don't move past the bottom 
		newCamY = PLAYFIELD_HEIGHT - SCR_HEIGHT ;
	}

	// Store the values
	camPosX = newCamX;
	camPosY = newCamY;
	// Update the MAP position
	//MAP_scrollTo( map_a, camPosX, camPosY );
	REG_BG1HOFS = camPosX;
	REG_BG1VOFS = camPosY;
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

	

	int tick = 0;
	int shipFrame = 0;
	while (1) {
		VBlankIntrWait();
		scanKeys();
		u16 keys = keysHeld();

		if (keys & KEY_LEFT) {
			shipPosX--;
			if( shipPosX < -8 ) shipPosX = 0;
		}

		if (keys & KEY_RIGHT) {
			shipPosX++;
			if( shipPosX >= PLAYFIELD_WIDTH - PLAYER_WIDTH + 8 ) shipPosX = PLAYFIELD_WIDTH-PLAYER_WIDTH + 8;
		}

		if (keys & KEY_UP) {
			shipPosY--;
			if( shipPosY < 0 ) shipPosY = 0;
		}

		if (keys & KEY_DOWN) {
			shipPosY++;
			if( shipPosY >= PLAYFIELD_HEIGHT - PLAYER_HEIGHT + 8 ) shipPosY = PLAYFIELD_HEIGHT - PLAYER_HEIGHT + 8;
		}

		// udpate pos
		int x = shipPosX - camPosX;
		int y = shipPosY - camPosY;
		OAM_MEM[0].attr0 &= 0xff00;
		OAM_MEM[0].attr0 |= ( y & 0x00ff );
		OAM_MEM[0].attr1 &= 0xfe00;
		OAM_MEM[0].attr1 |= ( x & 0x00ff );


		tick++;
		if( tick % 5 == 0 ) {	
			shipFrame+=4;
			if( shipFrame > 28 ) shipFrame = 0;
			OAM_MEM[0].attr2 &= 0xfc00;
			OAM_MEM[0].attr2 |= OBJ_CHAR(shipFrame);
		}

		updateCameraPos();
	}
}


