
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_math.h"
#include "rays.h"
#include <float.h>


#include <gba_timers.h>

#define TIMER_ENABLE (1<<7) already in header as TIMER_START(BIT(7))

// prescaler values
#define TIMER_DIV_1     0
#define TIMER_DIV_64    1
#define TIMER_DIV_256   2
#define TIMER_DIV_1024  3


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


  // set graphics mode
  SetMode( MODE_3 | BG2_ON );

  // init
  init_scene();
  Vec3 O =(Vec3) {0.0f, 0.0f, 0.0f}; // camera location

  // setup timer
  REG_TM2CNT_H = 0;
  REG_TM3CNT_H = 0;
  // have timer 2 take appx 1 second to overflow
  //  65536 - 16384 = 49152 -> 49152 == C000
  REG_TM2CNT_L = 0xC000;   // change start value for timer 2 to 49152
  REG_TM3CNT_L = 0;        // set start value for timer 3 to 0.. will increment when timer 2 overflows (appx every second)

  // use bit7 (TIMER_START) to start the timer
  // use bits 0&1 to set division 16.777216 MHz / 1024 ~= 16384 ticks/sec (TIMER_DIV_1024 == 3 )
  REG_TM2CNT_H = TIMER_START | TIMER_DIV_1024;
  // use bit2 (TIMER_COUNT) uses preceding timer (2) to tell timer 3 to tick up
  REG_TM3CNT_H = TIMER_START | TIMER_COUNT;


  // do tracing.
  for( int x = -CANVAS_WIDTH/2; x < CANVAS_WIDTH/2; ++x ) {
    for( int y = -CANVAS_HEIGHT/2; y < CANVAS_HEIGHT/2; ++y ) {
      Vec3 D = canvas_to_viewport( x,y); // get direction to point on viewport
                                         // t_min == 1 ( we care about ojbects past theview port
                                         // t_max = FLT_MAX (or infinity)
      u16 color = trace_ray( O, D, 1, FLT_MAX );
      put_pixel(x, y, color );
    }
  }
  uint16_t curr_time_s = REG_TM3CNT_L;

    iprintf("seconds: %u\n", curr_time_s );

  while (1) {
    VBlankIntrWait();
  }
}


