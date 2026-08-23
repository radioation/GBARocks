
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>

#include <gba_sio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UART_SEND_DATA_FLAG ( 1 << 4 )
#define UART_RECEIVE_DATA_FLAG ( 1 << 5 )

#define UART_DATA_LENGTH_8 (1 << 7 )
#define UART_SEND_ENABLE (1 << 10 )
#define UART_RECEIVE_ENABLE (1 << 11 )



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



    /*
Not particularly useful:

Init & Initback
The content of the FIFO is reset when FIFO is disabled in UART mode, thus, when entering UART mode initially set FIFO=disabled.
The Send/Receive enable bits must be reset before switching from UART mode into another SIO mode!
     */


/*
4000134h - RCNT (R) - Mode Selection, in Normal/Multiplayer/UART modes (R/W)
  Bit   Expl.
  0-3   Undocumented (current SC,SD,SI,SO state, as for General Purpose mode)
  4-8   Not used     (Should be 0, bits are read/write-able though)
  9-13  Not used     (Always 0, read only)
  14    Not used     (Should be 0, bit is read/write-able though)
  15    Must be zero (0) for Normal/Multiplayer/UART modes
*/    
    /*
gba_sio.h:
    #define R_UART                  0x0000
    */
    REG_RCNT = 0;  // - 15    Must be zero (0) for Normal/Multiplayer/UART modes


/*
4000128h - SCCNT_L - SIO Control, usage in UART Mode (R/W)
  Bit   Expl.
  0-1   Baud Rate  (0-3: 9600,38400,57600,115200 bps)
  2     CTS Flag   (0=Send always/blindly, 1=Send only when SC=LOW)
  3     Parity Control (0=Even, 1=Odd)
  4     Send Data Flag      (0=Not Full,  1=Full)    (Read Only)
  5     Receive Data Flag   (0=Not Empty, 1=Empty)   (Read Only)
  6     Error Flag          (0=No Error,  1=Error)   (Read Only)
  7     Data Length         (0=7bits,   1=8bits)
  8     FIFO Enable Flag    (0=Disable, 1=Enable)
  9     Parity Enable Flag  (0=Disable, 1=Enable)
  10    Send Enable Flag    (0=Disable, 1=Enable)
  11    Receive Enable Flag (0=Disable, 1=Enable)
  12    Must be "1" for UART mode
  13    Must be "1" for UART mode
  14    IRQ Enable          (0=Disable, 1=IRQ when any Bit 4/5/6 become set)
  15    Not used            (Read only, always 0)

*/

  
    /*
gba_sio.h:
    #define SIO_115200              0x0003
    #define SIO_UART                0x3000  // UART communication mode 
    */
    REG_SIOCNT = SIO_115200 | SIO_UART | UART_DATA_LENGTH_8 | UART_SEND_ENABLE | UART_RECEIVE_ENABLE;
                

    // send a string over serial port
    const char *mesg = "GBA ROCKS!\r\n";
    for( int i=0; i < strlen( mesg ); ++i ) {
        //   4     Send Data Flag      (0=Not Full,  1=Full)    (Read Only)
        while (REG_SIOCNT & UART_SEND_DATA_FLAG) {}  // loop while Send Data flag is full (1)
        REG_SIODATA8 = mesg[i]; // use SIODATA8 to send a byte
    }


    while (1) {
    
  // 5     Receive Data Flag   (0=Not Empty, 1=Empty)   (Read Only)
        while( REG_SIOCNT & UART_RECEIVE_DATA_FLAG ) {}  // loop while Receive Data flag is empty (1)
        uint8_t c = REG_SIODATA8;  // read a byte
        iprintf("%c", c );

        // echo it back
        while (REG_SIOCNT & UART_SEND_DATA_FLAG) {}  // loop while Send Data flag is full (1)
        REG_SIODATA8 = c; // use SIODATA8 to send a byte

        VBlankIntrWait();
    }


}


