
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>

#include <gba_sio.h>

#include <stdio.h>
#include <stdlib.h>



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

       - Initialize RCNT Bit 14-15 and SIOCNT Bit 12-13 to select Multi-Player mode.
       - Read SIOCNT Bit 3 to verify that all GBAs are in Multi-Player mode.
       - Read SIOCNT Bit 2 to detect whether this is the Parent/Master unit.

     */

    ///////////////////////////////////////////////////////////////
    // - Initialize RCNT Bit 14-15 and SIOCNT Bit 12-13 to select Multi-Player mode.
    /*

       4000134h - RCNT (R) - Mode Selection, in Normal/Multiplayer/UART modes (R/W)
       Bit   Expl.
       0-3   Undocumented (current SC,SD,SI,SO state, as for General Purpose mode)
       4-8   Not used     (Should be 0, bits are read/write-able though)
       9-13  Not used     (Always 0, read only)
       14    Not used     (Should be 0, bit is read/write-able though)
       15    Must be zero (0) for Normal/Multiplayer/UART modes

        Note: Even though undocumented, many Nintendo games are using Bit 0 to
        test current SC state in multiplay mode.


        4000128h - SIOCNT - SIO Control, usage in MULTI-PLAYER Mode (R/W)
          Bit   Expl.
          0-1   Baud Rate     (0-3: 9600,38400,57600,115200 bps)
          2     SI-Terminal   (0=Parent, 1=Child)                  (Read Only)
          3     SD-Terminal   (0=Bad connection, 1=All GBAs Ready) (Read Only)
          4-5   Multi-Player ID     (0=Parent, 1-3=1st-3rd child)  (Read Only)
          6     Multi-Player Error  (0=Normal, 1=Error)            (Read Only)
          7     Start/Busy Bit      (0=Inactive, 1=Start/Busy) (Read Only for Slaves)
          8-11  Not used            (R/W, should be 0)
          12    Must be "0" for Multi-Player mode
          13    Must be "1" for Multi-Player mode
          14    IRQ Enable          (0=Disable, 1=Want IRQ upon completion)
          15    Not used            (Read only, always 0)
        The ID Bits are undefined until the first transfer has completed.

     */
    REG_RCNT = 0;  // - Initialize RCNT Bit 14-15 and SIOCNT Bit 12-13 to select Multi-Player mode.
    REG_SIOCNT = ( 1 <<13) | 3;     // I'm also using the low 2 bits for baud rate (0-3, 3= 115200 )
                                    // only master needs to set baud rate.

    ///////////////////////////////////////////////////////////////
    // - Read SIOCNT Bit 3 to verify that all GBAs are in Multi-Player mode.
    while( ! (REG_SIOCNT & ( 1 << 3 ))  );

    ///////////////////////////////////////////////////////////////
    // - Read SIOCNT Bit 2 to detect whether this is the Parent/Master unit.
    bool is_child = REG_SIOCNT & ( 1 << 2 );

    short id = -1; 

/*
Recommended Transmission Procedure
- Write outgoing data to SIODATA_SEND.
- Master must set Start bit.
- All units must process received data in SIOMULTI0-3 when transfer completed.
- After the first successful transfer, ID Bits in SIOCNT are valid.
- If more data is to be transferred, repeat procedure.
The parent unit blindly sends data regardless of whether childs have already processed old data/supplied new data. So, parent unit might be required to insert delays between each transfer, and/or perform error checking.
Also, slave units may signalize that they are not ready by temporarily switching into another communication mode (which does not output SD High, as Multi-Player mode does during inactivity).
*/
    while (1) {
        // - Write outgoing data to SIODATA_SEND.
        REG_SIOMLT_SEND = id;

        iprintf("\x1b[1;1H");
        //- Master must set Start bit.
        if( !is_child ) {
            iprintf("Master start\n");
            REG_SIOCNT |= SIO_START;
        }
       
        // not explicity stated in procedure but recall for SIOCNT
        //  `7     Start/Busy Bit      (0=Inactive, 1=Start/Busy) (Read Only for Slaves)`
        while ( REG_SIOCNT & SIO_START ) {
        }
 
        // - All units must process received data in SIOMULTI0-3 when transfer completed.
        u16 p0 = REG_SIOMULTI0;
        u16 p1 = REG_SIOMULTI1;
        u16 p2 = REG_SIOMULTI2;
        u16 p3 = REG_SIOMULTI3;


        if( id >=0 ) {

            iprintf("I am %d\n", id );
            iprintf("  p0  %d      \n", p0 );
            iprintf("  p1  %d      \n", p1 );
            iprintf("  p2  %d      \n", p2);
            iprintf("  p3  %d      \n", p3 );

        } else {
            // - After the first successful transfer, ID Bits in SIOCNT are valid.
            id =  (REG_SIOCNT & 0x0030) >> 4; 
        }
        // - If more data is to be transferred, repeat procedure.

        VBlankIntrWait();
    }


}


