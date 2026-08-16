
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>


// constants
#define MAX_SHOTS 15
#define MAX_UFOS 9 
#define MAX_UFO_SHOTS  18

#define LEFT_EDGE  0
#define RIGHT_EDGE  320
#define TOP_EDGE 0
#define BOTTOM_EDGE 224


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

	consoleDemoInit();

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	iprintf("\x1b[10;10HHello World!\n");

	while (1) {
		VBlankIntrWait();
	}
}


