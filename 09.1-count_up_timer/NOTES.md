* even at the largest division, a single 16-bit timer would reset at  4 seconds.
  `65535/16384 = 4`

* If we want longer durations we can chain timers to gether with bit 2 of 
  `TM*CNT_H`. Enabling count-up timing means the current timer will only 
  tick up when the preceding one overflows.


```c
    // we want timer 2 to take appx 1 second to overflow. Using 1024 divisor
    //  65536 - 16384 = 49152 -> 49152 == C000
    REG_TM2CNT_L = 0xC000;   // change start value for timer 2 to 49152 
    REG_TM3CNT_L = 0;        // set start value for timer 3 to 0.. will increment when timer 2 overflows (appx every second)

    // use bit7 (TIMER_START) to start the timer
    // use bits 0&1 to set division 16.777216 MHz / 1024 ~= 16384 ticks/sec (TIMER_DIV_1024 == 3 )
    REG_TM2CNT_H = TIMER_START | TIMER_DIV_1024;
    // use bit2 (TIMER_COUNT) uses preceding timer (2) to tell timer 3 to tick up
    REG_TM3CNT_H = TIMER_START | TIMER_COUNT; 
```



