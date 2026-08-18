
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_dma.h>
#include <gba_sprites.h>
#include <stdio.h>
#include <stdlib.h>

#include "myfix.h"

#include "ufo.h"
#include "ship.h"
#include "boom.h"
#include "shot.h"
#include "space.h"
#include "gb_rock.h"

#define OAM_MEM ((volatile OBJATTR *)0x07000000)

// the play 
struct CP_SPRITE {
	myfix  obj_index;
	myfix  pos_x;
	myfix  pos_y;
	myfix  vel_x;
	myfix  vel_y;

	int hitbox_x1;
	int hitbox_y1;
	int hitbox_x2;
	int hitbox_y2;

	bool active;

};

enum SHIP_STATE {
    ship_dead = 0,
    ship_live = 1,
    ship_warping_in = 2,
    ship_warp_pressed = 4,
};
myfix ship_warp_pos_x = MYFIX(0.0);
myfix ship_warp_pos_y = MYFIX(0.0);
u8 ship_state = ship_dead;
static u16 score = 0;
static s16 lives = 3;
static s16 ship_ticks = 0;
u8 shipDir = 0;
const u8 angleStep = 2;


/////////////////////////////////////////////////////////////////////////////////
// Define player constants
#define PLAYER_WIDTH        16
#define PLAYER_HEIGHT       16
#define PLAYER_SHOT_WIDTH   4
#define PLAYER_SHOT_HEIGHT  4
#define SHOT_OFFSET_X       2
#define SHOT_OFFSET_Y       2
#define MAX_PLAYER_SHOTS    4
#define PLAYER_SHOT_TIME    50




/////////////////////////////////////////////////////////////////////////////////
// Define enemy constants
#define MAX_OBJECTS         45
// leave 7 of the objs for UFOs and player shots  ( 4 player shots, 1 UFOS, and 2 ufo shots )
#define MAX_ROCKS           38
#define MAX_EXPLOSIONS      5


//#define UFO_SPAWN_TIME  0x02f8
#define UFO_SPAWN_TIME  300
#define UFO_SHOT_TICKS  60


struct CP_SPRITE shipSprite; // only one player
struct CP_SPRITE shipShots[MAX_PLAYER_SHOTS];
struct CP_SPRITE rocks[MAX_ROCKS];
struct CP_SPRITE ufo;     // single ufo at a time.
struct CP_SPRITE ufoShot; // single shot



/////////////////////////////////////////////////////////////////////////////////
// Define map/world constants
#define MAP_WIDTH           480
#define MAP_HEIGHT          480


#define SCR_WIDTH           240
#define SCR_HEIGHT          160

#define CAM_BOUNDARY        40

s16 camPosX; // relative to total world map
s16 camPosY; // relative to total world map

static u8 tick = 0; // just a commn tick for everyone to use
s16 delayTicks = -1; // count down times to delay events, like rocks at the start of a level.
u16 level = 1;
bool levelStarted = false;

enum GAME_MODE {
    attract_mode,
    play_mode,
    dead_mode,
    high_score_mode // someday
};

u8 game_mode = attract_mode;


void spawnShip() {
    shipSprite.pos_x = MYFIX( MAP_WIDTH / 2 );
    shipSprite.pos_y = MYFIX( MAP_HEIGHT / 2 );
    ship_state = ship_live | ship_warping_in;
    ship_ticks = 0; // reset for warp in timing
}



int createShipShots(int start_ind) {
	myfix xpos = MYFIX(-16);
	myfix ypos = MYFIX(166);

	int curr_ind = start_ind;
	for( int i=0; i < MAX_PLAYER_SHOTS; ++i ) {
		shipShots[i].pos_x = xpos;
		shipShots[i].pos_y = ypos;
		shipShots[i].vel_x = MYFIX(0);
		shipShots[i].vel_y = MYFIX(0);
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
	//u16 up = keysUp();
    u32 keys = ~(REG_KEYINPUT);
    //if( game_mode == play_mode ) {
    if( true ) {
        if( keys & KEY_LEFT ) {
            shipDir -= angleStep ;
            shipDir + 8;
            // total of 16 frames but 256 dirs. 1 frame has to cover 16 directions
            u8 tmpDir =  shipDir >> 4;
            OAM_MEM[0].attr2 = ATTR2_PALETTE(1) | OBJ_CHAR(4 + (tmpDir << 2)) | OBJ_PRIORITY(0);

        }
        if( keys & KEY_RIGHT ) {
            shipDir += angleStep;
            shipDir + 8;
            // need to update frame 
            u8 tmpDir = shipDir >> 4;
            OAM_MEM[0].attr2 = ATTR2_PALETTE(1) | OBJ_CHAR(4 + (tmpDir << 2)) | OBJ_PRIORITY(0);
            
        }

    } else {
      //  if( down == KEY_START ) {
      //      //clear_enemy_objs();
      //      level = 1;
      //      lives = 1;
      //      game_mode = play_mode;
      //      spawnShip();
      //      delayTicks = 128;
      //  }
    }


/*
	if ( down & KEY_A ) {
		int addedShot = 0;
		for( int i=0; i < MAX_PLAYER_SHOTS; ++i ) {
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

*/


}



int createUFO(int start_ind) {
    int curr_ind = start_ind;
    ufo.pos_x = MYFIX(0);
    ufo.pos_y = MYFIX(160);
    ufo.vel_x = MYFIX(0);
    ufo.vel_y = MYFIX(0);
    ufo.active = true;
    ufo.hitbox_x1 = 0;
    ufo.hitbox_y1 = 0;
    ufo.hitbox_x2 = 16;
    ufo.hitbox_y2 = 16;

    ufo.obj_index = curr_ind;
    OAM_MEM[ufo.obj_index].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(ufo.pos_y);	
    OAM_MEM[ufo.obj_index].attr1 = ATTR1_SIZE_16  | OBJ_X(ufo.pos_x);
    OAM_MEM[ufo.obj_index].attr2 = ATTR2_PALETTE(2) | OBJ_CHAR(68) | OBJ_PRIORITY(0);

    curr_ind++;
    return curr_ind;
}

void update() {
    if( game_mode == play_mode ) {
        if( ship_state & ship_live ) {
            if( ship_state & ship_warping_in ) {
                ship_ticks++;
                if( ship_ticks < 150 ) {
                    if ( ship_ticks % 3 ) {
                        //     SPR_setVisibility(ship_sprite, VISIBLE);
                    } else {
                        //    SPR_setVisibility(ship_sprite, HIDDEN);
                    }
                } else {
                    //   SPR_setVisibility(ship_sprite, VISIBLE);
                    ship_state ^= ship_warping_in;
                }
            }
            //Position the ship
            shipSprite.pos_x += shipSprite.vel_x;
            shipSprite.pos_y += shipSprite.vel_y;
            //Check horizontal bounds
            if(shipSprite.pos_x < MYFIX(0)){
                shipSprite.pos_x = MYFIX(0);
                shipSprite.vel_x = -shipSprite.vel_x;
            } else if(shipSprite.pos_x  > MAP_WIDTH){
                shipSprite.pos_x = MAP_WIDTH  - PLAYER_WIDTH ;
                shipSprite.vel_x = -shipSprite.vel_x;
            }
            //Check vert bounds
            if(shipSprite.pos_y < MYFIX(0)){
                shipSprite.pos_y = MYFIX(0);
                shipSprite.vel_y = -shipSprite.vel_y;
            } else if(shipSprite.pos_y  > MAP_HEIGHT){
                shipSprite.pos_y = MAP_HEIGHT  - PLAYER_WIDTH ;
                shipSprite.vel_y = -shipSprite.vel_y;
            }


        }
    }



    // update player position
    OAM_MEM[0].attr0 &= 0xff00;
    OAM_MEM[0].attr0 |= ( fixToInt(shipSprite.pos_y) & 0x00ff );
    OAM_MEM[0].attr1 &= 0xfe00;
    OAM_MEM[0].attr1 |= ( fixToInt(shipSprite.pos_x) & 0x00ff );


    //    // shots
    //    for( u16 i=0; i < MAX_PLAYER_SHOTS; ++i ) {
    //        if( shipShots[i].active == true ) {
    //            shipShots[i].pos_x +=  shipShots[i].vel_x;
    //            shipShots[i].pos_y +=  shipShots[i].vel_y;
    //            if(shipShots[i].pos_y  < 0 ) {
    //                shipShots[i].pos_x = -16;
    //                shipShots[i].pos_y = 166;
    //                shipShots[i].vel_x = 0;
    //                shipShots[i].vel_y = 0;
    //                shipShots[i].active = false;
    //            }
    //            //SPR_setPosition(shipShots[i].sprite,shipShots[i].pos_x,shipShots[i].pos_y);
    //            OAM_MEM[shipShots[i].obj_index].attr0 &= 0xff00;
    //            OAM_MEM[shipShots[i].obj_index].attr0 |= ( shipShots[i].pos_y & 0x00ff );
    //            OAM_MEM[shipShots[i].obj_index].attr1 &= 0xfe00;
    //            OAM_MEM[shipShots[i].obj_index].attr1 |= ( shipShots[i].pos_x & 0x00ff );
    //        } else {
    //            OAM_MEM[shipShots[i].obj_index].attr0 &= 0xff00;
    //            OAM_MEM[shipShots[i].obj_index].attr0 |=  166 ;
    //            OAM_MEM[shipShots[i].obj_index].attr1 &= 0xfe00;
    //            OAM_MEM[shipShots[i].obj_index].attr1 |=  0 ;
    //        }
    //    }
    //
    //
    //    // update ufos
    //    if( ufo.active == true ) {
    //        // actually not needed here, maybe later
    //        OAM_MEM[ufo.obj_index].attr0 &= 0xff00;
    //        OAM_MEM[ufo.obj_index].attr0 |= ( ufo.pos_y & 0x00ff );
    //        OAM_MEM[ufo.obj_index].attr1 &= 0xfe00;
    //        OAM_MEM[ufo.obj_index].attr1 |= ( ufo.pos_x & 0x00ff );
    //    } else {
    //        //SPR_setPosition( ufo.sprite, -32, 230 );
    //        OAM_MEM[ufo.obj_index].attr0 &= 0xff00;
    //        OAM_MEM[ufo.obj_index].attr0 |=  166 ;
    //        OAM_MEM[ufo.obj_index].attr1 &= 0xfe00;
    //        OAM_MEM[ufo.obj_index].attr1 |=  0 ;
    //    }

}



void checkCollisions() {
    /*
       for( u16 i=0; i < MAX_UFOS; ++i ) {
       if( ufo.active == true ) {
    // check if ship has hit
    if( (ufo.pos_x + ufo.hitbox_x1) < (shipSprite.pos_x + shipSprite.hitbox_x2) &&
    (ufo.pos_x + ufo.hitbox_x2) > (shipSprite.pos_x + shipSprite.hitbox_x1) &&
    (ufo.pos_y + ufo.hitbox_y1) < (shipSprite.pos_y + shipSprite.hitbox_y2) &&
    (ufo.pos_y + ufo.hitbox_y2) > (shipSprite.pos_y + shipSprite.hitbox_y1)  ) 
    {
    ufo.active = false;
    }

    for( u16 j=0; j < MAX_PLAYER_SHOTS; ++j ) {
    if(
    shipShots[j].active == true &&
    (ufo.pos_x + ufo.hitbox_x1) < (shipShots[j].pos_x + shipShots[j].hitbox_x2) &&
    (ufo.pos_x + ufo.hitbox_x2) > (shipShots[j].pos_x + shipShots[j].hitbox_x1) &&
    (ufo.pos_y + ufo.hitbox_y1) < (shipShots[j].pos_y + shipShots[j].hitbox_y2) &&
    (ufo.pos_y + ufo.hitbox_y2) > (shipShots[j].pos_y + shipShots[j].hitbox_y1)  ) 
    {
    ufo.active = false;
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
     */


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
    dmaCopy( spacePal, BG_PALETTE, spacePalLen );
    dmaCopy( shotPal, SPRITE_PALETTE, shotPalLen );
    dmaCopy( shipPal, SPRITE_PALETTE + 16, shipPalLen );
    dmaCopy( ufoPal, SPRITE_PALETTE + 32, ufoPalLen );
    dmaCopy( boomPal, SPRITE_PALETTE + 48, boomPalLen );
    //dmaCopy( gb_rockPal, SPRITE_PALETTE + 64, gb_rockPalLen ); same as UFO.

    // tiles
    dmaCopy( spaceTiles, TILE_BASE_ADR(0), spaceTilesLen );
    dmaCopy( spaceMap, MAP_BASE_ADR(8), spaceMapLen );

    dmaCopy( shotTiles, OBJ_BASE_ADR, shotTilesLen );
    dmaCopy( shipTiles, OBJ_BASE_ADR + 128, shipTilesLen );
    dmaCopy( ufoTiles, OBJ_BASE_ADR + 128 + 2048, ufoTilesLen );
    dmaCopy( boomTiles, OBJ_BASE_ADR + 128 + 2048  + 1024, boomTilesLen );


    REG_BG1CNT = ( BG_SIZE_3 | BG_16_COLOR | TILE_BASE(0) | MAP_BASE(8) );


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
    lastIndex = createUFO(lastIndex);	
    //lastIndex = createUFOShot(lastIndex);	


    while (1) {
        VBlankIntrWait();
        readKeys();
        update();
        checkCollisions();
    }
}


