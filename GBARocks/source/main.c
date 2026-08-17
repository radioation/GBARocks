
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_dma.h>
#include <gba_sprites.h>
#include <stdio.h>
#include <stdlib.h>

#include "ufo.h"
#include "ship.h"
#include "boom.h"
#include "shot.h"

// constants
#define MAX_SHOTS 15
#define MAX_UFOS 9 
#define MAX_UFO_SHOTS  18

#define LEFT_EDGE  0
#define RIGHT_EDGE  232
#define TOP_EDGE 0
#define BOTTOM_EDGE 144


#define OAM_MEM ((volatile OBJATTR *)0x07000000)

// the play 
struct CP_SPRITE {
	int obj_index;
	int pos_x;
	int pos_y;
	int vel_x;
	int vel_y;

	int hitbox_x1;
	int hitbox_y1;
	int hitbox_x2;
	int hitbox_y2;

	bool active;

};


struct CP_SPRITE shipSprite;
struct CP_SPRITE shipShots[MAX_SHOTS];

struct CP_SPRITE ufos[MAX_UFOS];
struct CP_SPRITE ufoShots[MAX_UFO_SHOTS];


int createShipShots(int start_ind) {
	int xpos = -16;
	int ypos = 166;

	int curr_ind = start_ind;
	for( int i=0; i < MAX_SHOTS; ++i ) {
		shipShots[i].pos_x = xpos;
		shipShots[i].pos_y = ypos;
		shipShots[i].vel_x = 0;
		shipShots[i].vel_y = 0;
		shipShots[i].active = false;
		shipShots[i].hitbox_x1 = 2;
		shipShots[i].hitbox_y1 = 2;
		shipShots[i].hitbox_x2 = 6;
		shipShots[i].hitbox_y2 = 6;

		shipShots[i].obj_index = curr_ind;
		OAM_MEM[shipShots[i].obj_index].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(shipShots[i].pos_y);	
		OAM_MEM[shipShots[i].obj_index].attr1 = ATTR1_SIZE_8  | OBJ_X(shipShots[i].pos_x);
		OAM_MEM[shipShots[i].obj_index].attr2 = ATTR2_PALETTE(0) | OBJ_CHAR(1) | OBJ_PRIORITY(0);

		curr_ind++;
	}
	return curr_ind;

}

void readKeys() {
	scanKeys();

	u16 down = keysDown();
	u16 up = keysUp();

	if ( down & KEY_A ) {
		int addedShot = 0;
		for( int i=0; i < MAX_SHOTS; ++i ) {
			if( shipShots[i].active == false ) {
				shipShots[i].active = true;
				// set its starting position
				shipShots[i].pos_x = shipSprite.pos_x+4;
				shipShots[i].pos_y = shipSprite.pos_y;
				switch( addedShot ) {            
					case 0:
						shipShots[i].vel_x =  0;
						shipShots[i].vel_y = -6;
						break;

					case 1:
						shipShots[i].vel_x = -2;
						shipShots[i].vel_y = -5;
						break;

					case 2:
						shipShots[i].vel_x =  2;
						shipShots[i].vel_y = -5;
						break;

					case 3:
						shipShots[i].vel_x = -3;
						shipShots[i].vel_y = -3;
						break;

					case 4:
						shipShots[i].vel_x =  3;
						shipShots[i].vel_y = -3;
						break;


				}
				++addedShot;
				if( addedShot >= 5 ) {
					break;
				}
			}
		}

	}


	if( down & KEY_LEFT ) {
		shipSprite.vel_x = -1;
	} else if ( up & KEY_LEFT ) {
		shipSprite.vel_x = 0;
	}
	if( down & KEY_RIGHT ) {
		shipSprite.vel_x = 1;
	} else if ( up & KEY_RIGHT ) {
		shipSprite.vel_x = 0;
	}

	if( down & KEY_UP ) {
		shipSprite.vel_y = -1;
	} else if ( up & KEY_UP ) {
		shipSprite.vel_y = 0;
	}
	if( down & KEY_DOWN ) {
		shipSprite.vel_y = 1;
	} else if ( up & KEY_DOWN ) {
		shipSprite.vel_y = 0;
	}

	// respawn 
	if (down & KEY_R) {
		for( u16 i=0; i < MAX_UFOS; ++i ) {
			ufos[i].active = true;
		}
	}

}



int createUFOs(int start_ind) {
	int curr_ind = start_ind;
	s16 ufo_pos_x = 50;
	s16 ufo_pos_y = 14;

	s16 xpos = 50;
	s16 ypos = 14;
	for( u16 i=0; i < MAX_UFOS; ++i ) {
		if( i == 5 ) {
			ypos = ufo_pos_y + 40;
			xpos = ufo_pos_x + 15;
		} 
		ufos[i].pos_x = xpos;
		ufos[i].pos_y = ypos;
		ufos[i].vel_x = 0;
		ufos[i].vel_y = 0;
		ufos[i].active = true;
		ufos[i].hitbox_x1 = 0;
		ufos[i].hitbox_y1 = 0;
		ufos[i].hitbox_x2 = 16;
		ufos[i].hitbox_y2 = 16;

		ufos[i].obj_index = curr_ind;
		OAM_MEM[ufos[i].obj_index].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(ufos[i].pos_y);	
		OAM_MEM[ufos[i].obj_index].attr1 = ATTR1_SIZE_16  | OBJ_X(ufos[i].pos_x);
		OAM_MEM[ufos[i].obj_index].attr2 = ATTR2_PALETTE(2) | OBJ_CHAR(68) | OBJ_PRIORITY(0);
		xpos += 30;
		curr_ind++;

	}

}

void update() {
	//Check horizontal bounds
	if(shipSprite.pos_x < LEFT_EDGE){
		shipSprite.pos_x = LEFT_EDGE;
		shipSprite.vel_x = -shipSprite.vel_x;
	} else if(shipSprite.pos_x + (shipSprite.hitbox_x2 - shipSprite.hitbox_x1 ) > RIGHT_EDGE){
		shipSprite.pos_x = RIGHT_EDGE - (shipSprite.hitbox_x2 - shipSprite.hitbox_x1) ;
		shipSprite.vel_x = -shipSprite.vel_x;
	}


	//Check vertical bounds
	if(shipSprite.pos_y < TOP_EDGE){
		shipSprite.pos_y = TOP_EDGE;
		shipSprite.vel_y = -shipSprite.vel_y;
	} else if(shipSprite.pos_y + (shipSprite.hitbox_y2 - shipSprite.hitbox_y1 ) > BOTTOM_EDGE){
		shipSprite.pos_y = BOTTOM_EDGE - ( shipSprite.hitbox_y2 - shipSprite.hitbox_y1 );
		shipSprite.vel_y = -shipSprite.vel_y;
	}

	//Position the ship
	shipSprite.pos_x += shipSprite.vel_x;
	shipSprite.pos_y += shipSprite.vel_y;


	// update player position
	OAM_MEM[0].attr0 &= 0xff00;
	OAM_MEM[0].attr0 |= ( shipSprite.pos_y & 0x00ff );
	OAM_MEM[0].attr1 &= 0xfe00;
	OAM_MEM[0].attr1 |= ( shipSprite.pos_x & 0x00ff );


	// shots
	for( u16 i=0; i < MAX_SHOTS; ++i ) {
		if( shipShots[i].active == true ) {
			shipShots[i].pos_x +=  shipShots[i].vel_x;
			shipShots[i].pos_y +=  shipShots[i].vel_y;
			if(shipShots[i].pos_y  < 0 ) {
				shipShots[i].pos_x = -16;
				shipShots[i].pos_y = 166;
				shipShots[i].vel_x = 0;
				shipShots[i].vel_y = 0;
				shipShots[i].active = false;
			}
			//SPR_setPosition(shipShots[i].sprite,shipShots[i].pos_x,shipShots[i].pos_y);
			OAM_MEM[shipShots[i].obj_index].attr0 &= 0xff00;
			OAM_MEM[shipShots[i].obj_index].attr0 |= ( shipShots[i].pos_y & 0x00ff );
			OAM_MEM[shipShots[i].obj_index].attr1 &= 0xfe00;
			OAM_MEM[shipShots[i].obj_index].attr1 |= ( shipShots[i].pos_x & 0x00ff );
		} else {
			OAM_MEM[shipShots[i].obj_index].attr0 &= 0xff00;
			OAM_MEM[shipShots[i].obj_index].attr0 |=  166 ;
			OAM_MEM[shipShots[i].obj_index].attr1 &= 0xfe00;
			OAM_MEM[shipShots[i].obj_index].attr1 |=  0 ;
		}
	}


	// update ufos
	for( u16 i=0; i < MAX_UFOS; ++i ) {
		if( ufos[i].active == true ) {
			// actually not needed here, maybe later
			OAM_MEM[ufos[i].obj_index].attr0 &= 0xff00;
			OAM_MEM[ufos[i].obj_index].attr0 |= ( ufos[i].pos_y & 0x00ff );
			OAM_MEM[ufos[i].obj_index].attr1 &= 0xfe00;
			OAM_MEM[ufos[i].obj_index].attr1 |= ( ufos[i].pos_x & 0x00ff );
		} else {
			//SPR_setPosition( ufos[i].sprite, -32, 230 );
			OAM_MEM[ufos[i].obj_index].attr0 &= 0xff00;
			OAM_MEM[ufos[i].obj_index].attr0 |=  166 ;
			OAM_MEM[ufos[i].obj_index].attr1 &= 0xfe00;
			OAM_MEM[ufos[i].obj_index].attr1 |=  0 ;
		}
	}

}



void checkCollisions() {
	// likely expensive, I know.
	for( u16 i=0; i < MAX_UFOS; ++i ) {
		if( ufos[i].active == true ) {
			// check if ship has hit
			if( (ufos[i].pos_x + ufos[i].hitbox_x1) < (shipSprite.pos_x + shipSprite.hitbox_x2) &&
					(ufos[i].pos_x + ufos[i].hitbox_x2) > (shipSprite.pos_x + shipSprite.hitbox_x1) &&
					(ufos[i].pos_y + ufos[i].hitbox_y1) < (shipSprite.pos_y + shipSprite.hitbox_y2) &&
					(ufos[i].pos_y + ufos[i].hitbox_y2) > (shipSprite.pos_y + shipSprite.hitbox_y1)  ) 
			{
				ufos[i].active = false;
			}

			for( u16 j=0; j < MAX_SHOTS; ++j ) {
				if(
						shipShots[j].active == true &&
						(ufos[i].pos_x + ufos[i].hitbox_x1) < (shipShots[j].pos_x + shipShots[j].hitbox_x2) &&
						(ufos[i].pos_x + ufos[i].hitbox_x2) > (shipShots[j].pos_x + shipShots[j].hitbox_x1) &&
						(ufos[i].pos_y + ufos[i].hitbox_y1) < (shipShots[j].pos_y + shipShots[j].hitbox_y2) &&
						(ufos[i].pos_y + ufos[i].hitbox_y2) > (shipShots[j].pos_y + shipShots[j].hitbox_y1)  ) 
				{
					ufos[i].active = false;
					shipShots[j].active = false;
				}
			}
		}
	}
	for( u16 i=0; i < MAX_UFO_SHOTS; ++i ) {
		if( ufoShots[i].active == true ) {
			if( (ufoShots[i].pos_x + ufoShots[i].hitbox_x1) < (shipSprite.pos_x + shipSprite.hitbox_x2) &&
					(ufoShots[i].pos_x + ufoShots[i].hitbox_x2) > (shipSprite.pos_x + shipSprite.hitbox_x1) &&
					(ufoShots[i].pos_y + ufoShots[i].hitbox_y1) < (shipSprite.pos_y + shipSprite.hitbox_y2) &&
					(ufoShots[i].pos_y + ufoShots[i].hitbox_y2) > (shipSprite.pos_y + shipSprite.hitbox_y1)  ) 
			{
				ufoShots[i].active = false;
			}
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

	REG_DISPCNT = ( MODE_0|BG1_ON | OBJ_ENABLE | OBJ_1D_MAP );	

	// setup palettes
	dmaCopy( shotPal, SPRITE_PALETTE, shotPalLen );
	dmaCopy( shipPal, SPRITE_PALETTE + 16, shipPalLen );
	dmaCopy( ufoPal, SPRITE_PALETTE + 32, ufoPalLen );
	dmaCopy( boomPal, SPRITE_PALETTE + 48, boomPalLen );

	// tiles
	dmaCopy( shotTiles, OBJ_BASE_ADR, shotTilesLen );
	dmaCopy( shipTiles, OBJ_BASE_ADR + 128, shipTilesLen );
	dmaCopy( ufoTiles, OBJ_BASE_ADR + 128 + 2048, ufoTilesLen );
	dmaCopy( boomTiles, OBJ_BASE_ADR + 128 + 2048  + 1024, boomTilesLen );


	// clear things out
	for(int i = 0; i < 128; i++) {
		OAM_MEM[i].attr0 = ATTR0_DISABLED;
	}

	// initial ship
	shipSprite.pos_x = 112;
	shipSprite.pos_y = 100;
	shipSprite.vel_x = 0;
	shipSprite.vel_y = 0;
	shipSprite.active = true;
	shipSprite.hitbox_x1 = 0;
	shipSprite.hitbox_y1 = 0;
	shipSprite.hitbox_x2 = 16;
	shipSprite.hitbox_y2 = 16;
	shipSprite.obj_index = 0;
	OAM_MEM[0].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(shipSprite.pos_y);	
	OAM_MEM[0].attr1 = ATTR1_SIZE_16 | OBJ_X(shipSprite.pos_x);
	OAM_MEM[0].attr2 = ATTR2_PALETTE(1) | OBJ_CHAR(4) | OBJ_PRIORITY(0);

	int lastIndex = createShipShots(1);
	createUFOs(lastIndex);	
	//createUFOShots();	


	while (1) {
		VBlankIntrWait();
		readKeys();
		update();
		checkCollisions();
	}
}


