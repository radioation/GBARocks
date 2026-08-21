
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

// Define SIO registers for clarity
#define REG_SIODATA8 (*(volatile u16*)0x0400012A)
#define REG_SIOCNT   (*(volatile u16*)0x04000128)
#define REG_RCNT     (*(volatile u16*)0x04000134)


#define MASTER 1
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
       Initialization

       First, initialize RCNT register. 

       Second, set mode/clock bits in SIOCNT with startbit cleared. 
     * For master: select internal clock, and (in most cases) specify 256KHz as transfer rate. 
     * For slave: select external clock, the local transfer rate selection is then
     ignored, as the transfer rate is supplied by the remote GBA (or other
     computer, which might supply custom transfer rates).

     Third, set the startbit in SIOCNT with mode/clock bits unchanged.
     */

    // ansi escape sequence to set print co-ordinates
    // /x1b[line;columnH

    ///////////////////////////////////////////////////////////////
    // FIRST INITIALIZE RCNT REGISTER
    /*
       4000134h - RCNT (R) - Mode Selection, in Normal/Multiplayer/UART modes (R/W)
       Bit   Expl.
       0-3   Undocumented (current SC,SD,SI,SO state, as for General Purpose mode)
       4-8   Not used     (Should be 0, bits are read/write-able though)
       9-13  Not used     (Always 0, read only)
       14    Not used     (Should be 0, bit is read/write-able though)
       15    Must be zero (0) for Normal/Multiplayer/UART modes
     */
    REG_RCNT = 0;  // srsly?

    ///////////////////////////////////////////////////////////////
    // SECOND SET MODE/CLOCK BITS IN SIOCNT WITH STARTBIT CLEARD
    /*
       4000128h - SIOCNT - SIO Control, usage in NORMAL Mode (R/W)
       Bit   Expl.
       0     Shift Clock (SC)        (0=External, 1=Internal)
       1     Internal Shift Clock    (0=256KHz, 1=2MHz)
       2     SI State (opponents SO) (0=Low, 1=High/None) --- (Read Only)
       3     SO during inactivity    (0=Low, 1=High) (applied ONLY when Bit7=0)
       4-6   Not used                (Read only, always 0 ?)
       7     Start Bit               (0=Inactive/Ready, 1=Start/Active)
       8-11  Not used                (R/W, should be 0)
       12    Transfer Length         (0=8bit, 1=32bit)
       13    Must be "0" for Normal Mode
       14    IRQ Enable              (0=Disable, 1=Want IRQ upon completion)
       15    Not used                (Read only, always 0)
     */
#ifdef MASTER
    // INTERNAL CLOCK (1) for bit 0
    // 256KHz (0) for bit 1
    // Starbit cleared (0)  bit 7
    // Transfer LEnght 0=8bit bit 12
    // bit 13 must be 0 for normal mode
    // IRQ disabled : bit 14
    REG_SIOCNT = 1; 
#else
    // EXTERNAL CLOCK (0) for bit 0
    // Starbit cleared (0)  bit 7
    // Transfer LEnght 0=8bit bit 12
    // bit 13 must be 0 for normal mode
    // IRQ disabled : bit 14
    REG_SIOCNT = 0; 
#endif

#define SIO_SI ( 1 << 2 )
#define SIO_START       (1 << 7)

    ///////////////////////////////////////////////////////////////
    // THIRD, set the startbit in SIOCNT with mode/clock bits unchanged

    // didn't I laredy do this above?
    REG_SIOCNT |= SIO_START;


#ifdef MASTER
    /*
       Recommended Communication Procedure for MASTER unit (internal clock)
       - Initialize data which is to be sent to slave.
       - Wait for SI to become LOW (slave ready). (Check timeout here!)
       - Set Start flag.
       - Wait for IRQ (or for Start bit to become zero).
       - Process received data.
       - Repeat procedure if more data is to be transferred.
     */
    int sent = 0;
    while (1) {
        // initialize data which is to be sent to slave
        REG_SIODATA8 = 100+sent;

        // wait for SI to be come low
        while( REG_SIOCNT & SIO_SI ) {}

        // set start flag
        REG_SIOCNT |= SIO_START;

        // - Wait for IRQ (or for Start bit to become zero).
        while( REG_SIOCNT & SIO_START ) {}

        // -- Process received data.
        uint8_t got = REG_SIODATA8; 
        iprintf("M GOT %d\n", got );
        sent++;
        if( sent > 5 ) {
            sent = 0; 
            iprintf("\x1b[1;1H");
        }
        VBlankIntrWait();
    }
#else
    /*
       - Initialize data which is to be sent to master.
       - Set Start=0 and SO=0 (SO=LOW indicates that slave is (almost) ready).
       - Set Start=1 and SO=1 (SO=HIGH indicates not ready, applied after transfer).
       (Expl. Old SO=LOW kept output until 1st clock bit received).
       (Expl. New SO=HIGH is automatically output at transfer completion).
       - Set SO to LOW to indicate that master may start now.
       - Wait for IRQ (or for Start bit to become zero). (Check timeout here!)
       - Process received data.
       - Repeat procedure if more data is to be transferred.
     */

    int sent = 0;
    while (1) {
        // Initialize data which is to be sent to master.
        REG_SIODATA8 = 200+sent;

        //  Set Start=0 and SO=0 (SO=LOW indicates that slave is (almost) ready).
        //  start is bit 7, so is bit 3 0xFF77            
        REG_SIOCNT &= 0xFF77;

        //- Set Start=1 and SO=1 (SO=HIGH indicates not ready, applied after transfer).
        REG_SIOCNT |= 0x0088;


        // - Set SO to LOW to indicate that master may start now.
        REG_SIOCNT &= 0xFFF7;

        // - Wait for IRQ (or for Start bit to become zero).
        while( REG_SIOCNT & SIO_START ) {}

        // -- Process received data.
        uint8_t got = REG_SIODATA8; 
        iprintf("S GOT %d\n", got );
        sent++;
        if( sent > 5 ) {
            sent = 0; 
            iprintf("\x1b[1;1H");
        }
        VBlankIntrWait();
    }

#endif


}


