
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
#include "gb_mid_rock.h"
#include "gb_small_rock.h"

#define OAM_MEM ((volatile OBJATTR *)0x07000000)




// the play 
struct CP_SPRITE {
    myfix  obj_index;
    myfix  pos_x;
    myfix  pos_y;
    myfix  vel_x;
    myfix  vel_y;

    myfix hitbox_x1;
    myfix hitbox_y1;
    myfix hitbox_x2;
    myfix hitbox_y2;

    bool active;
    int tile_index;
    int tile_step;
    int frame_count;
    int frame;
    int frame_delay;
    int ticks;
};

enum SHIP_STATE {
    ship_dead = 0,
    ship_live = 1,
    ship_warping_in = 2,
    ship_warp_pressed = 4,
};
myfix shipAccelX = MYFIX(0.0);
myfix shipAccelY = MYFIX(0.0);
//myfix ship_warp_pos_x = MYFIX(0.0);
//myfix ship_warp_pos_y = MYFIX(0.0);
u8 ship_state = ship_dead;
static int score = 0;
static int lives = 3;
static int ship_ticks = 0;
u8 shipDir = 0;
const u8 angleStep = 2;

const int32_t shotOffset = 0;
const int32_t shipOffset = 128;
const int32_t ufoOffset = 128 + 2048;
const int32_t boomOffset = 128 + 2048 + 1024;
const int32_t rockOffset = 128 + 2048 + 1024 + 4608;
const int32_t midRockOffset = 128 + 2048 + 1024 + 4608 + 4096;
const int32_t smallRockOffset = 128 + 2048 + 1024 + 4608 + 4096 + 1024;


/////////////////////////////////////////////////////////////////////////////////
// Define player constants
#define PLAYER_WIDTH        16
#define PLAYER_HEIGHT       16
#define PLAYER_SHOT_WIDTH   4
#define PLAYER_SHOT_HEIGHT  4
#define SHOT_OFFSET_X       2
#define SHOT_OFFSET_Y       2
#define MAX_PLAYER_SHOTS    4
#define PLAYER_SHOT_TIME    80




/////////////////////////////////////////////////////////////////////////////////
// Define enemy constants
#define MAX_ROCKS           10
#define MAX_EXPLOSIONS      8


//#define UFO_SPAWN_TIME  0x02f8
#define UFO_SPAWN_TIME  300
#define UFO_SHOT_TICKS  60


struct CP_SPRITE shipSprite; // only one player
struct CP_SPRITE shipShots[MAX_PLAYER_SHOTS];
struct CP_SPRITE rocks[MAX_ROCKS];
struct CP_SPRITE ufo;     // single ufo at a time.
struct CP_SPRITE ufoShot; // single shot
struct CP_SPRITE explosions[MAX_EXPLOSIONS];



/////////////////////////////////////////////////////////////////////////////////
// Define map/world constants
#define PLAYFIELD_WIDTH           480
#define PLAYFIELD_HEIGHT          480


#define SCR_WIDTH           240
#define SCR_HEIGHT          160

#define CAMERA_PADDING        40

int camPosX; // relative to total world map
int camPosY; // relative to total world map

static u8 tick = 0; // just a common tick for everyone to use
int delayTicks = -1; // count down times to delay events, like rocks at the start of a level.
int level = 1;
bool levelStarted = false;

enum GAME_MODE {
    attract_mode,
    play_mode,
    dead_mode,
    high_score_mode // someday
};

u8 game_mode = attract_mode;





void spawnShip() {
    shipSprite.pos_x = MYFIX( PLAYFIELD_WIDTH / 2 );
    shipSprite.pos_y = MYFIX( PLAYFIELD_HEIGHT / 2 );
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
        shipShots[i].hitbox_x1 = MYFIX(2);
        shipShots[i].hitbox_y1 = MYFIX(2);
        shipShots[i].hitbox_x2 = MYFIX(6);
        shipShots[i].hitbox_y2 = MYFIX(6);
        shipShots[i].ticks = 0;

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

    int down = keysDown();
    //int up = keysUp();
    u32 keys = ~(REG_KEYINPUT);
    //if( game_mode == play_mode ) {
    if( true ) {
        if( keys & KEY_LEFT ) {
            shipDir -= angleStep ;
            // total of 16 frames but 256 dirs. 1 frame has to cover 16 directions
            u8 tmpDir =  shipDir >> 4;
            OAM_MEM[0].attr2 = ATTR2_PALETTE(1) | OBJ_CHAR(4 + (tmpDir << 2)) | OBJ_PRIORITY(0);

        }
        if( keys & KEY_RIGHT ) {
            shipDir += angleStep;
            // need to update frame 
            u8 tmpDir = shipDir >> 4;
            OAM_MEM[0].attr2 = ATTR2_PALETTE(1) | OBJ_CHAR(4 + (tmpDir << 2)) | OBJ_PRIORITY(0);

        }
        if( down & KEY_A ) {
            for( int i =0; i < MAX_PLAYER_SHOTS; ++i ) {
                if( shipShots[i].active == false ) {
                    shipShots[i].active = true;
                    shipShots[i].ticks = 0;
                    shipShots[i].pos_x = shipSprite.pos_x + MYFIX( 4 );
                    shipShots[i].pos_y = shipSprite.pos_y + MYFIX( 4 );
                    shipShots[i].vel_x = shipSprite.vel_x + ( thrustX[shipDir] << 4 );
                    shipShots[i].vel_y = shipSprite.vel_y + ( thrustY[shipDir] << 4 );
                    break;
                }
            }

        }

        if( keys & KEY_UP ) {
            shipAccelX = thrustX[shipDir];
            shipSprite.vel_x += shipAccelX;
            if( shipSprite.vel_x > MYFIX(0.0) && shipAccelX > MYFIX(0.0) && shipSprite.vel_x >  maxSpeedX[shipDir] ) {
                shipSprite.vel_x = maxSpeedX[shipDir];
                shipAccelX = MYFIX(0.0);
            } else if ( shipSprite.vel_x < MYFIX(0) && shipAccelX < MYFIX(0.0) && shipSprite.vel_x <  maxSpeedX[shipDir] ) {
                shipSprite.vel_x = maxSpeedX[shipDir];
                shipAccelX = MYFIX(0.0);
            }
            shipAccelY += thrustY[shipDir];
            shipSprite.vel_y += shipAccelY;
            if( shipSprite.vel_y > MYFIX(0.0) && shipAccelY > MYFIX(0.0) && shipSprite.vel_y >  maxSpeedY[shipDir] ) {
                shipSprite.vel_y = maxSpeedY[shipDir];
                shipAccelY = MYFIX(0.0);
            } else if ( shipSprite.vel_y < MYFIX(0.0) && shipAccelY < MYFIX(0.0) && shipSprite.vel_y < maxSpeedY[shipDir] ) {
                shipSprite.vel_y = maxSpeedY[shipDir];
                shipAccelY = MYFIX(0.0);
            }




        } else {
            // Decelerate without atan()/atan2()
            // not thrusting, check x and y movemtn components
            //  and turn orn acceleration to counter. 
            //  admittedly not great, but OK~ish for this project.
            if( shipSprite.vel_x > MYFIX(0.0) ) {
                shipAccelX = MYFIX(-0.03);
                shipSprite.vel_x += shipAccelX;
                if( shipSprite.vel_x <= MYFIX(0.0) ) {
                    shipSprite.vel_x = MYFIX(0.0);
                    shipAccelX = MYFIX(0.0);
                }
            } else if( shipSprite.vel_x < MYFIX(0.0) ) {
                shipAccelX = MYFIX(0.03);
                shipSprite.vel_x += shipAccelX;
                if( shipSprite.vel_x >= MYFIX(0.0) ) {
                    shipSprite.vel_x = MYFIX(0.0);
                    shipAccelX = MYFIX(0.0);
                }
            } 
            if( shipSprite.vel_y > MYFIX(0.0) ) {
                shipAccelY = MYFIX(-0.03);
                shipSprite.vel_y += shipAccelY;
                if( shipSprite.vel_y <= MYFIX(0.0) ) {
                    shipSprite.vel_y = MYFIX(0.0);
                    shipAccelX = MYFIX(0.0);
                }
            } else if( shipSprite.vel_y < MYFIX(0.0) ) {
                shipAccelY = MYFIX(0.03);
                shipSprite.vel_y += shipAccelY;
                if( shipSprite.vel_y >= MYFIX(0.0) ) {
                    shipSprite.vel_y = MYFIX(0.0);
                    shipAccelX = MYFIX(0.0);
                }
            } 
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




}



int createUFO(int start_ind) {
    int curr_ind = start_ind;
    ufo.pos_x = MYFIX(40);
    ufo.pos_y = MYFIX(40);
    ufo.vel_x = MYFIX(0);
    ufo.vel_y = MYFIX(0);
    ufo.active = true;
    ufo.hitbox_x1 = MYFIX(0);
    ufo.hitbox_y1 = MYFIX(0);
    ufo.hitbox_x2 = MYFIX(16);
    ufo.hitbox_y2 = MYFIX(16);
    ufo.tile_step = 4;
    ufo.frame_count = 8;
    ufo.frame = random()%8;
    ufo.frame_delay = 2;

    ufo.obj_index = curr_ind;

    OAM_MEM[ufo.obj_index].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(fixToInt(ufo.pos_y));	
    OAM_MEM[ufo.obj_index].attr1 = ATTR1_SIZE_16  | OBJ_X(fixToInt(ufo.pos_x));
    OAM_MEM[ufo.obj_index].attr2 = ATTR2_PALETTE(2) | OBJ_CHAR(ufoOffset/32) | OBJ_PRIORITY(0);

    curr_ind++;
    return curr_ind;
}

int createUFOShot(int start_ind) {
    int curr_ind = start_ind;
    ufoShot.pos_x = MYFIX(0);
    ufoShot.pos_y = MYFIX(0);
    ufoShot.vel_x = MYFIX(0);
    ufoShot.vel_y = MYFIX(0);
    ufoShot.active = false;
    ufoShot.hitbox_x1 = MYFIX(2);
    ufoShot.hitbox_y1 = MYFIX(2);
    ufoShot.hitbox_x2 = MYFIX(6);
    ufoShot.hitbox_y2 = MYFIX(6);
    ufoShot.tile_step = 1;
    ufoShot.frame_count = 4;
    ufoShot.frame = 0;
    ufoShot.frame_delay = 2;

    ufoShot.obj_index = curr_ind;

    OAM_MEM[ufoShot.obj_index].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(ufoShot.pos_y);	
    OAM_MEM[ufoShot.obj_index].attr1 = ATTR1_SIZE_8  | OBJ_X(ufoShot.pos_x);
    OAM_MEM[ufoShot.obj_index].attr2 = ATTR2_PALETTE(0) | OBJ_CHAR(shotOffset/32) | OBJ_PRIORITY(0);

    curr_ind++;
    return curr_ind;
}


void update() {
    //if( game_mode == play_mode ) 
    if( true ) {
        //if( ship_state & ship_live ) 
        if( true ) {
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
            } else if(shipSprite.pos_x  > MYFIX(PLAYFIELD_WIDTH)){
                shipSprite.pos_x = MYFIX(PLAYFIELD_WIDTH  - PLAYER_WIDTH) ;
                shipSprite.vel_x = -shipSprite.vel_x;
            }
            //Check vert bounds
            if(shipSprite.pos_y < MYFIX(0)){
                shipSprite.pos_y = MYFIX(0);
                shipSprite.vel_y = -shipSprite.vel_y;
            } else if(shipSprite.pos_y  > MYFIX(PLAYFIELD_HEIGHT)){
                shipSprite.pos_y = MYFIX(PLAYFIELD_HEIGHT  - PLAYER_WIDTH) ;
                shipSprite.vel_y = -shipSprite.vel_y;
            }


        }
    }



    // update player position
    int y = fixToInt(shipSprite.pos_y) - camPosY;
    if( y < 256 && y >= 0 ) {
        OAM_MEM[0].attr0 &= 0xff00;
        OAM_MEM[0].attr0 |= ( y & 0x00ff );
    }
    int x = fixToInt(shipSprite.pos_x) - camPosX;
    OAM_MEM[0].attr1 &= 0xfe00;
    OAM_MEM[0].attr1 |= ( x & 0x01ff );


    // shots
    for( int i=0; i < MAX_PLAYER_SHOTS; ++i ) {
        if( shipShots[i].active == true ) {
            shipShots[i].pos_x +=  shipShots[i].vel_x;
            shipShots[i].pos_y +=  shipShots[i].vel_y;
            x = fixToInt( shipShots[i].pos_x ) - camPosX;
            y = fixToInt( shipShots[i].pos_y ) - camPosY;
            shipShots[i].ticks++;
            if(shipShots[i].ticks > PLAYER_SHOT_TIME ){
                shipShots[i].active = false;
            }
            //SPR_setPosition(shipShots[i].sprite,shipShots[i].pos_x,shipShots[i].pos_y);
                OAM_MEM[shipShots[i].obj_index].attr0 &= 0xff00;
            if( y < 160 && y >= -8 ) {
                OAM_MEM[shipShots[i].obj_index].attr0 |= y & 0x00ff;
            } else {
                OAM_MEM[shipShots[i].obj_index].attr0 |= 166;
            }
            OAM_MEM[shipShots[i].obj_index].attr1 &= 0xfe00;
            OAM_MEM[shipShots[i].obj_index].attr1 |= x & 0x01ff;
        } else {
            OAM_MEM[shipShots[i].obj_index].attr0 &= 0xff00;
            OAM_MEM[shipShots[i].obj_index].attr0 |=  166 ;
            OAM_MEM[shipShots[i].obj_index].attr1 &= 0xfe00;
            OAM_MEM[shipShots[i].obj_index].attr1 |=  0 ;
        }
    }

    // ROCKS
    for( int i=0; i < MAX_ROCKS; ++i ) {
        if( rocks[i].active == true ) {
            rocks[i].pos_x +=  rocks[i].vel_x;
            if( rocks[i].pos_x < MYFIX(-32) ) {
                rocks[i].pos_x = PLAYFIELD_WIDTH;
            }else if( rocks[i].pos_x > MYFIX(PLAYFIELD_WIDTH) ) {
                rocks[i].pos_x = MYFIX(0);
            }


            rocks[i].pos_y +=  rocks[i].vel_y;
            if( rocks[i].pos_y < MYFIX(-32) ) {
                rocks[i].pos_y = PLAYFIELD_HEIGHT;
            }else if( rocks[i].pos_y > MYFIX(PLAYFIELD_HEIGHT) ) {
                rocks[i].pos_y = MYFIX(0);
            }


            x = fixToInt( rocks[i].pos_x ) - camPosX;
            y = fixToInt( rocks[i].pos_y ) - camPosY;

            if( tick % rocks[i].frame_delay == 0 ) {                
                rocks[i].frame++;
                if( rocks[i].frame >= rocks[i].frame_count ) {
                    rocks[i].frame = 0;
                }
            }


            //SPR_setPosition(rocks[i].sprite,rocks[i].pos_x,rocks[i].pos_y);
                OAM_MEM[rocks[i].obj_index].attr0 &= 0xff00;
            if( y < 160 && y > -32)  {
                OAM_MEM[rocks[i].obj_index].attr0 |= ( y & 0x00ff );
            } else {
                OAM_MEM[rocks[i].obj_index].attr0 |= 166;
            }
            OAM_MEM[rocks[i].obj_index].attr1 &= 0xfe00;
            OAM_MEM[rocks[i].obj_index].attr1 |= ( x & 0x01ff );
            OAM_MEM[rocks[i].obj_index].attr2 = ATTR2_PALETTE(2) | OBJ_CHAR( rocks[i].frame*16 +  rockOffset/32) | OBJ_PRIORITY(0);
        } else {
            OAM_MEM[rocks[i].obj_index].attr0 &= 0xff00;
            OAM_MEM[rocks[i].obj_index].attr0 |=  166 ;
            OAM_MEM[rocks[i].obj_index].attr1 &= 0xfe00;
            OAM_MEM[rocks[i].obj_index].attr1 |=  0 ;
        }
    }

    //    // update ufos
    if( ufo.active == true ) {
        if( tick % ufo.frame_delay == 0 ) {                
            ufo.frame++;
            if( ufo.frame >= ufo.frame_count ) {
                ufo.frame = 0;
            }
        }
        x = fixToInt( ufo.pos_x ) - camPosX;
        y = fixToInt( ufo.pos_y ) - camPosY;
        OAM_MEM[ufo.obj_index].attr0 &= 0xff00;
        if( y < 160 && y >= -16  ) {
        OAM_MEM[ufo.obj_index].attr0 |= ( y & 0x00ff );
        } else {
        OAM_MEM[ufo.obj_index].attr0 |= 166;
        }
        OAM_MEM[ufo.obj_index].attr1 &= 0xfe00;
        OAM_MEM[ufo.obj_index].attr1 |= ( x & 0x01ff );
        OAM_MEM[ufo.obj_index].attr2 = ATTR2_PALETTE(2) | OBJ_CHAR( ufo.frame*ufo.tile_step +  ufoOffset/32) | OBJ_PRIORITY(0);
    } else {
        //SPR_setPosition( ufo.sprite, -32, 230 );
        OAM_MEM[ufo.obj_index].attr0 &= 0xff00;
        OAM_MEM[ufo.obj_index].attr0 |=  166 ;
        OAM_MEM[ufo.obj_index].attr1 &= 0xfe00;
        OAM_MEM[ufo.obj_index].attr1 |=  0 ;
    }

    for( int i=0; i < MAX_EXPLOSIONS ; ++i ) {
        if( explosions[i].active == true ) {
            x = fixToInt( explosions[i].pos_x ) - camPosX;
            y = fixToInt( explosions[i].pos_y ) - camPosY;
            if( explosions[i]. ticks < 9 ) {
                OAM_MEM[explosions[i].obj_index].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(y);
                OAM_MEM[explosions[i].obj_index].attr1 = ATTR1_SIZE_32  | OBJ_X(x);
                OAM_MEM[explosions[i].obj_index].attr2 = ATTR2_PALETTE(3) | OBJ_CHAR( explosions[i].frame*16 +  boomOffset/32) | OBJ_PRIORITY(0);
                explosions[i].ticks++;
                explosions[i].frame++;

            } else {
                explosions[i].active = false;
        OAM_MEM[explosions[i].obj_index].attr0 &= 0xff00;
        OAM_MEM[explosions[i].obj_index].attr0 |=  166 ;
        OAM_MEM[explosions[i].obj_index].attr1 &= 0xfe00;
        OAM_MEM[explosions[i].obj_index].attr1 |=  0 ;
            }

        }
    }

}


int currentExplosion = 0;

static void showExplosion(myfix pos_x, myfix pos_y)
{
    if (explosions[currentExplosion].active == false)
    {
        // use it
        explosions[currentExplosion].active = true;
        explosions[currentExplosion].ticks = 0;
        explosions[currentExplosion].frame = 0;
        explosions[currentExplosion].pos_x = pos_x;
        explosions[currentExplosion].pos_y = pos_y;

        //SPR_setVisibility(explosion_sprites[currentExplosion], VISIBLE);
        //SPR_setPosition(explosion_sprites[currentExplosion], x, y );

        //XGM_startPlayPCM(SND_EXPLOSION, 10, SOUND_PCM_CH3);

        // point to next explosion
        ++currentExplosion;
        if (currentExplosion >= MAX_EXPLOSIONS)
        {
            currentExplosion = 0;
        }
    }
}

void checkCollisions() {

    // check  
    for( int i=0; i < MAX_PLAYER_SHOTS; ++i ) {
        if( shipShots[i].active == true ) {
            // check if UFO hit.
            if(     ufo.active &&
                    (ufo.pos_x + ufo.hitbox_x1) < (shipShots[i].pos_x + shipShots[i].hitbox_x2) &&
                    (ufo.pos_x + ufo.hitbox_x2) > (shipShots[i].pos_x + shipShots[i].hitbox_x1) &&
                    (ufo.pos_y + ufo.hitbox_y1) < (shipShots[i].pos_y + shipShots[i].hitbox_y2) &&
                    (ufo.pos_y + ufo.hitbox_y2) > (shipShots[i].pos_y + shipShots[i].hitbox_y1)  ) 
            {
                ufo.active = false;
                shipShots[i].active = false;
                showExplosion( ufo.pos_x-MYFIX(8), ufo.pos_y-MYFIX(8) );
            } 
            else 
            {   
                // check if rock hit
                for( int j=0; j < MAX_ROCKS; ++j ) {
                    if(     rocks[j].active &&
                            (rocks[j].pos_x + rocks[j].hitbox_x1) < (shipShots[i].pos_x + shipShots[i].hitbox_x2) &&
                            (rocks[j].pos_x + rocks[j].hitbox_x2) > (shipShots[i].pos_x + shipShots[i].hitbox_x1) &&
                            (rocks[j].pos_y + rocks[j].hitbox_y1) < (shipShots[i].pos_y + shipShots[i].hitbox_y2) &&
                            (rocks[j].pos_y + rocks[j].hitbox_y2) > (shipShots[i].pos_y + shipShots[i].hitbox_y1)  ) 
                    {
                        rocks[j].active = false;
                        shipShots[i].active = false;
                        showExplosion( rocks[j].pos_x, rocks[j].pos_y );
                        break;  // only do one
                    } 
                }
            }


        }
    }
    /*
       for( int i=0; i < MAX_UFOS; ++i ) {
       if( ufo.active == true ) {
    // check if ship has hit
    if( (ufo.pos_x + ufo.hitbox_x1) < (shipSprite.pos_x + shipSprite.hitbox_x2) &&
    (ufo.pos_x + ufo.hitbox_x2) > (shipSprite.pos_x + shipSprite.hitbox_x1) &&
    (ufo.pos_y + ufo.hitbox_y1) < (shipSprite.pos_y + shipSprite.hitbox_y2) &&
    (ufo.pos_y + ufo.hitbox_y2) > (shipSprite.pos_y + shipSprite.hitbox_y1)  ) 
    {
    ufo.active = false;
    }
     */
    /*}
      }
      for( int i=0; i < MAX_UFO_SHOTS; ++i ) {
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


static void updateCameraPos() {
    int px = fixToInt( shipSprite.pos_x);
    int py = fixToInt( shipSprite.pos_y);
    // figure out where the ship is.
    int playerScreenX = px - camPosX;
    int playerScreenY = py - camPosY;

    // Adjust new camera X position based on ship position
    int newCamX;
    // check if the ship X position is too close to the right edge of the screen
    if( playerScreenX > SCR_WIDTH - CAMERA_PADDING - PLAYER_WIDTH ) {
        newCamX = px - ( SCR_WIDTH - CAMERA_PADDING - PLAYER_WIDTH );
    } else if( playerScreenX < CAMERA_PADDING ) { // check if the ship is too close to the left
        newCamX = px - CAMERA_PADDING;
    } else {
        newCamX = camPosX; // no change to camera position.
    }

    // Adjust camera Y position based on ship position
    int newCamY;
    // check if the ship Y position is too close to the bottom edge of the screen
    if( playerScreenY > SCR_HEIGHT - CAMERA_PADDING - PLAYER_HEIGHT ) {
        newCamY = py - ( SCR_HEIGHT - CAMERA_PADDING - PLAYER_HEIGHT ) ;
    } else if( playerScreenY < CAMERA_PADDING ) {  // is ship too close to the top of the screen?
        newCamY = py - CAMERA_PADDING;
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




void createRock(u8 i, int rockType, myfix x, myfix y ) {
}

int createRocks(int start_ind) {
    int curr_ind = start_ind;
    for( int i=0; i < MAX_ROCKS; ++i ) {
        rocks[i].pos_x = MYFIX(random()%(PLAYFIELD_WIDTH-32));  // random starting position for rock sprites
        rocks[i].pos_y = MYFIX(random()%(PLAYFIELD_HEIGHT-32));

        // use ranodm direction for rock motion
        int rot = random() % 256; 
        myfix vel = MYFIX(0.8);
        rocks[i].vel_x = fix_mul( vel, thrustX[rot]  );
        rocks[i].vel_y = fix_mul( vel, thrustY[rot]  );
        rocks[i].active = true;
        rocks[i].hitbox_x1 = MYFIX(2);
        rocks[i].hitbox_y1 = MYFIX(2);
        rocks[i].hitbox_x2 = MYFIX(30);
        rocks[i].hitbox_y2 = MYFIX(30);
        rocks[i].tile_index = rockOffset;
        rocks[i].tile_step = 16;
        rocks[i].frame_count = 8;
        rocks[i].frame = random()%8;
        rocks[i].frame_delay = 10;

        //rocks[i].sprite = SPR_addSprite( &rock, -32, -32, TILE_ATTR( PAL3, 0, FALSE, FALSE ));
        rocks[i].obj_index = curr_ind;
        OAM_MEM[rocks[i].obj_index].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(fixToInt(rocks[i].pos_y));
        OAM_MEM[rocks[i].obj_index].attr1 = ATTR1_SIZE_32  | OBJ_X(fixToInt(rocks[i].pos_x));
        OAM_MEM[rocks[i].obj_index].attr2 = ATTR2_PALETTE(2) | OBJ_CHAR( rocks[i].frame*16 +  rockOffset/32) | OBJ_PRIORITY(0);
        //	SPR_setAnim( rocks[i].sprite, 0 );
        curr_ind++;
    }
    return curr_ind;

}

int createExplosions(int start_ind) {
    int curr_ind = start_ind;
    for( int i=0; i < MAX_EXPLOSIONS; ++i ) {
        explosions[i].pos_x = MYFIX(-32);
        explosions[i].pos_y = MYFIX(-32);

        explosions[i].vel_x = MYFIX(0); 
        explosions[i].vel_y = MYFIX(0);
        explosions[i].active = false;
        explosions[i].hitbox_x1 = MYFIX(0); // not used.
        explosions[i].hitbox_y1 = MYFIX(0);
        explosions[i].hitbox_x2 = MYFIX(0);
        explosions[i].hitbox_y2 = MYFIX(0);
        explosions[i].tile_index = boomOffset;
        explosions[i].tile_step = 16;
        explosions[i].frame_count = 8;
        explosions[i].frame = 0;
        explosions[i].frame_delay = 2;

        explosions[i].obj_index = curr_ind;
        OAM_MEM[explosions[i].obj_index].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(fixToInt(explosions[i].pos_y));
        OAM_MEM[explosions[i].obj_index].attr1 = ATTR1_SIZE_32  | OBJ_X(fixToInt(explosions[i].pos_x));
        OAM_MEM[explosions[i].obj_index].attr2 = ATTR2_PALETTE(3) | OBJ_CHAR( explosions[i].frame*16 +  boomOffset/32) | OBJ_PRIORITY(0);
        curr_ind++;
    }
    return curr_ind;

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

    //consoleDebugInit(DebugDevice_NOCASH);
    int x = 123;
    int y = 45;

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
    dmaCopy( shipTiles, OBJ_BASE_ADR + shipOffset, shipTilesLen );
    dmaCopy( ufoTiles, OBJ_BASE_ADR + ufoOffset, ufoTilesLen );
    dmaCopy( boomTiles, OBJ_BASE_ADR + boomOffset, boomTilesLen );
    dmaCopy( gb_rockTiles, OBJ_BASE_ADR + rockOffset, gb_rockTilesLen );
    dmaCopy( gb_mid_rockTiles, OBJ_BASE_ADR + midRockOffset, gb_mid_rockTilesLen );
    dmaCopy( gb_small_rockTiles, OBJ_BASE_ADR + smallRockOffset, gb_small_rockTilesLen );


    REG_BG1CNT = ( BG_SIZE_3 | BG_16_COLOR | TILE_BASE(0) | MAP_BASE(8) );


    // clear things out
    for(int i = 0; i < 128; i++) {
        OAM_MEM[i].attr0 = ATTR0_DISABLED;
    }

    // initial ship
    shipSprite.pos_x = MYFIX(112);
    shipSprite.pos_y = MYFIX(100);
    shipSprite.vel_x = MYFIX(0);
    shipSprite.vel_y = MYFIX(0);
    shipSprite.active = true;
    shipSprite.hitbox_x1 = 0;
    shipSprite.hitbox_y1 = 0;
    shipSprite.hitbox_x2 = 16;
    shipSprite.hitbox_y2 = 16;
    shipSprite.obj_index = 0;
    OAM_MEM[0].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(MYFIX(shipSprite.pos_y));	
    OAM_MEM[0].attr1 = ATTR1_SIZE_16 | OBJ_X(MYFIX(shipSprite.pos_x));
    OAM_MEM[0].attr2 = ATTR2_PALETTE(1) | OBJ_CHAR(shipOffset/32) | OBJ_PRIORITY(0);

    int lastIndex = createExplosions(1);	
    lastIndex = createUFO(lastIndex);	
    lastIndex = createRocks(lastIndex);	
    lastIndex = createUFOShot(lastIndex);	
    lastIndex = createShipShots(lastIndex);

    //iprintf("\x1b[1;1H");
    //iprintf("tX: %d iTX: %d   \n", thrustX[0], fixToInt(thrustX[0]) );
    //iprintf("tY: %d iTY: %d   \n", thrustY[0], fixToInt(thrustY[0]) );

    //myfix testval = MYFIX( 10.5 );
    //iprintf("tV: %d iTV: %d   \n", testval, fixToInt(testval));     

    while (1) {
        VBlankIntrWait();
        tick++;
        //        iprintf("\x1b[1;1H");
        //        iprintf("d: %d a: %d v: %d  \n", shipDir, fixToInt(shipAccelX), fixToInt( shipSprite.vel_x )); 
        readKeys();
        update();
        checkCollisions();
        updateCameraPos();
    }
}


