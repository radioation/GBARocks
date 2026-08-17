
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


/*
void createShipShots(int start_ind;) {
  int xpos = -16;
  int ypos = 230;

  for( u16 i=0; i < MAX_SHOTS; ++i ) {
    shipShots[i].pos_x = xpos;
    shipShots[i].pos_y = ypos;
    shipShots[i].vel_x = 0;
    shipShots[i].vel_y = 0;
    shipShots[i].active = FALSE;
    shipShots[i].hitbox_x1 = 0;
    shipShots[i].hitbox_y1 = 0;
    shipShots[i].hitbox_x2 = 8;
    shipShots[i].hitbox_y2 = 8;

    shipShots[i].obj_index;
    SPR_setAnim( shipShots[i].sprite, 2 );
  }

}
*/

void readKeys() {
	scanKeys();

	u16 down = keysDown();
	u16 up = keysUp();
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

	//createShipShots();
	//createUFOs();	
	//createUFOShots();	


	while (1) {
		VBlankIntrWait();
		readKeys();
		update();
		//checkCollisions();
	}
}


