
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

#include <gba_sio.h>
#define UART_SEND_DATA_FLAG ( 1 << 4 )
#define UART_RECEIVE_DATA_FLAG ( 1 << 5 )

#define UART_DATA_LENGTH_8 (1 << 7 )
#define UART_SEND_ENABLE (1 << 10 )
#define UART_RECEIVE_ENABLE (1 << 11 )

#define START_BYTE 0x7F
#define MAX_DATA_LEN 64
#define SERIAL_TIMEOUT 64

//////////////////////////////////////////////////////////
// COMMAND
typedef struct
{
  uint8_t command;
  // uint8_t sequence;   //
  uint8_t length;
  uint8_t data[MAX_DATA_LEN];
  uint8_t chksum; // chksum from GBA
  uint8_t calculated_chksum;
} cmd_packet;

// command/response byte
enum
{
  CMD_PING = 0x00,
  CMD_GET_VERSION = 0x01,
  CMD_TIME = 0x02,
  CMD_ECHO = 0x03,

  RESP_OK = 0x80,
  RESP_PING = 0x81,
  RESP_VERSION = 0x81,
  RESP_ECHO = 0x82,
  RESP_ERR = 0xFF,
};

//////////////////////////////////////////////////////////
// SERIAL 
typedef enum
{
  AWAIT_START, // default state.  exit to command when current byte is 7F
  GET_COMMAND, // read 1 byte and switch state to length
  GET_LENGTH,  // read 1 byte (set N) then switch state to data
  GET_DATA,    // read N bytes. Switch to chksum calculation.
  GET_CHKSUM,  // calc cksum, send response, switch to AWAIT_START state
} SerialState;


//////////////////////////////////////////////////////////
// 
void init_uart() {
    //
    REG_RCNT = 0;
    // 

//TODO: TURN ON FIFO 
//TODO: TURN ON CTS/RTS


    REG_SIOCNT = SIO_115200 | SIO_UART | UART_DATA_LENGTH_8 | UART_SEND_ENABLE | UART_RECEIVE_ENABLE;
}

void send_byte(u8 byte ) {
    // bit 4 == 1 waiting
    while (REG_SIOCNT & UART_SEND_DATA_FLAG) {}
    REG_SIODATA8  = byte;
}


// TODO add a timeout
static u8  get_byte( ) {
    // bit 4 == 1 waiting
    while (REG_SIOCNT & UART_RECEIVE_DATA_FLAG) {}
    return (u8) REG_SIODATA8;
}


uint8_t get_chksum( cmd_packet *cp ) {
    uint8_t chksum = 0;
    //cp->chksum ^= START_BYTE;
    chksum ^= cp->command; // not an actual checksum. XOR for now.
    chksum ^= cp->length;
    for(int i=0; i < cp->length; ++i ) {
        chksum ^= cp->data[i];
    }
    return chksum;
}


bool read_response(cmd_packet *cp)
{
  SerialState state = AWAIT_START;
  int current_byte = 0;
  while (true)
  {
    uint8_t b;
    //int count = uart_read_bytes(UART_NUM_1, &b, 1, pdMS_TO_TICKS(100));
    //iprintf("get byte");
    b = get_byte(); // TODO ADD A TIMEOUT and make b a param 
    //iprintf(" - got byte\n");
    //if (count)
    if (true)
    {
      switch (state)
      {
      case AWAIT_START:
        if (b == START_BYTE)
        {
          state = GET_COMMAND;
        }
        break;
      case GET_COMMAND:
        cp->command = b;
        state = GET_LENGTH;
        break;
      case GET_LENGTH:
        cp->length = b;
        current_byte = 0;
        if( cp->length > 0 ){
          state = GET_DATA;
        } else {
          state = GET_CHKSUM;
        }

        break;
      case GET_DATA:
        cp->data[current_byte] = b;
        current_byte++;
        if (current_byte > MAX_DATA_LEN)
        {
          state = GET_CHKSUM;
        }
        else if (current_byte == cp->length)
        {
          state = GET_CHKSUM;
        }
        break;
      case GET_CHKSUM:
        cp->chksum = b;
        cp->calculated_chksum = get_chksum(cp);
        iprintf("GOT CHKSUM %d %d\n", b, cp->calculated_chksum);
        state = AWAIT_START;
        return true;
      }
    }
  }
}


// send command 
void send_cp( cmd_packet *cp ) {
    // send SYNC
    send_byte( START_BYTE );
    // send CMD
    send_byte( cp->command );

    // send LEN
    send_byte( cp->length );

    // send payload
    for( int i=0; i < cp->length; ++i ) {
        send_byte( cp->data[i] );
    }
    // send chksum
    send_byte( cp->chksum );

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
    consoleDemoInit();
    init_uart();


    while (1) {
        VBlankIntrWait();
        scanKeys(); // 
        u16 down = keysDown();
        if( down & KEY_A ) {
            cmd_packet cp = { CMD_PING, 0, {}, 0  };            
            send_cp( &cp );
            cmd_packet rp;
            read_response(&rp);            
        }
        if( down & KEY_B ) {
            cmd_packet cp = { CMD_ECHO, 3, {'A', 'B','C'}, 0  };            
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );
            cmd_packet rp;
            read_response(&rp);            
        }
        if( down & KEY_R ) {
            cmd_packet cp = { CMD_ECHO, 10, {0,1,2,3,4,5,6,7,8,9}, 0  };            
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );
            cmd_packet rp;
            read_response(&rp);            
        }
    }
}


