
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_sprites.h>
#include <stdio.h>
#include <stdlib.h>

#define OAM_MEM ((volatile OBJATTR *)0x07000000)

const u16 sprite_tiles[] __attribute__( (aligned(4)) ) = {
	0x0000, 0x6000, 0x0000, 0x6300, 0x0000, 0x6200, 0x0000, 0xcc00, 0x0000, 0x2c00, 0x0000, 0x6260, 0x0000, 0x6166, 0x6000, 0x6126, 
	0x0000, 0x0000, 0x0003, 0x0000, 0x0003, 0x0000, 0x000c, 0x0000, 0x000c, 0x0000, 0x0062, 0x0000, 0x0662, 0x0000, 0x6632, 0x0000, 
	0x6600, 0x6121, 0x1600, 0x6121, 0x1660, 0x6121, 0x1160, 0x6121, 0x1160, 0x6121, 0x6200, 0x6121, 0x1000, 0x6121, 0x0000, 0x0021, 
	0x6232, 0x0006, 0x2232, 0x0006, 0x2232, 0x0066, 0x2232, 0x0062, 0x2232, 0x0062, 0x6232, 0x0003, 0x3232, 0x0004, 0x4230, 0x0000, 
};


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

	// BG2_ON          =       BIT(10),        /*!< enable background 2
	// OBJ_ENABLE              =       OBJ_ON,         /*!< enable sprites     
 	// OBJ_1D_MAP      =       BIT(6),         /*!< sprite 1 dimensional mapping      


        // get start of video ram
        // gba_base.h:#define      VRAM            0x06000000
        volatile u16 *vram = (volatile u16 *)VRAM;

        // set every pixel to dark blue
        for( int i=0; i < 240*160; ++i ) {
                vram[i] = RGB5( 0, 0, 4);
        }


	// set sprite palette colors. Couls also make an array and `memcpy( SPRITE_PALETTE, spritePal, spritePalLen )`;
	// gba_video.h:#define     SPRITE_PALETTE  ((u16 *)0x05000200)     // Sprite color table 	
	SPRITE_PALETTE[0] = RGB8( 0, 0, 0 );
	SPRITE_PALETTE[1] = RGB8( 255, 255, 255 );
	SPRITE_PALETTE[2] = RGB8( 204, 204, 204 );
	SPRITE_PALETTE[3] = RGB8( 136, 136, 136 );
	SPRITE_PALETTE[4] = RGB8( 68, 68, 68 );
	SPRITE_PALETTE[5] = RGB8( 255, 0, 0 );
	SPRITE_PALETTE[6] = RGB8( 153, 0, 0 );
	SPRITE_PALETTE[7] = RGB8( 0, 255, 0 );
	SPRITE_PALETTE[8] = RGB8( 0, 153, 0 );
	SPRITE_PALETTE[9] = RGB8( 0, 0, 255 );
	SPRITE_PALETTE[10] = RGB8( 0, 0, 153 );
	SPRITE_PALETTE[11] = RGB8( 48, 132, 92 );
	SPRITE_PALETTE[12] = RGB8( 240, 232, 72 );
	SPRITE_PALETTE[13] = RGB8( 52, 48, 116 );
	SPRITE_PALETTE[14] = RGB8( 188, 48, 108 );
	SPRITE_PALETTE[15] = RGB8( 40, 116, 196 );


	//  Copy sprite tile data to safe VRAM (offset by 512 tiles / 0x4000 bytes becase of mode 3
	// in tile mode 0x06010000. in bitmap mode 0x06014000
	//    duh: gba_sprites.h: #define OBJ_BASE_ADR            ((void *)(VRAM + 0x10000))  
	//         gba_sprites.h: #define BITMAP_OBJ_BASE_ADR     ((void *)(VRAM + 0x14000))q
	u16* sprite_vram = (u16*)( 0x06014000);
	//u16* sprite_vram = (u16*)( BITMAP_OBJ_BASE_ADR );
	for(int i = 0; i < 64; i++) {
		sprite_vram[i] = sprite_tiles[i];
	}


		VBlankIntrWait();

	// gba_sprites.h:#define   OAM                                     ((OBJATTR *)0x07000000)  
	for(int i = 0; i < 128; i++) {
		OAM_MEM[i].attr0 = ATTR0_DISABLED;
	}

	// Configure our single active sprite
	// #define ATTR0_COLOR_16                    (0<<13)
	// #define ATTR0_SQUARE      OBJ_SHAPE(SQUARE)
	// gba_sprites.h:#define OBJ_Y(m)                  ((m)&0x00ff)
	OAM_MEM[0].attr0 = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(40); // 4bpp, Square, Y=40


	// gba_sprites.h:#define ATTR1_SIZE_16         (1<<14)
	// gba_sprites.h:#define OBJ_X(m)                  ((m)&0x01ff)
	OAM_MEM[0].attr1 = ATTR1_SIZE_16 | OBJ_X(50);                  // 16x16 size, X=50

	OAM_MEM[0].attr2 = OBJ_CHAR(512) | OBJ_PRIORITY(0);           // Use Tile 512, highest priority
	//OAM[0].attr2 &= 0xfc00;
	///OAM[0].attr2 |= 512;

/*
volatile u16 *oam = (volatile u16 *)0x07000000;
VBlankIntrWait();


//oam[0] = 0x0028;   // attr0: normal, square, 4bpp, Y=40
//oam[1] = 0x4032;   // attr1: 16x16, X=50
//oam[2] = 0x0200;   // attr2: tile 512, priority 0, palette 0
	// Configure our single active sprite
	// #define ATTR0_COLOR_16                    (0<<13)
	// #define ATTR0_SQUARE      OBJ_SHAPE(SQUARE)
	// gba_sprites.h:#define OBJ_Y(m)                  ((m)&0x00ff)
	oam[0] = ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(40); // 4bpp, Square, Y=40


	// gba_sprites.h:#define ATTR1_SIZE_16         (1<<14)
	// gba_sprites.h:#define OBJ_X(m)                  ((m)&0x01ff)
	oam[1] = ATTR1_SIZE_16 | OBJ_X(50);                  // 16x16 size, X=50

	oam[2] = OBJ_CHAR(512) | OBJ_PRIORITY(0);           // Use Tile 512, highest priority
	//OAM[0].attr2 &= 0xfc00;
	///OAM[0].attr2 |= 512;
	////*/
	SetMode( MODE_3 | BG2_ON | OBJ_ENABLE | OBJ_1D_MAP );
	while (1) {
		VBlankIntrWait();
	}
}


