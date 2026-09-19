
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_timers.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gba_sio.h>
#define UART_SEND_DATA_FLAG ( 1 << 4 )
#define UART_RECEIVE_DATA_FLAG ( 1 << 5 )

#define UART_DATA_LENGTH_8 (1 << 7 )
#define UART_SEND_ENABLE (1 << 10 )
#define UART_RECEIVE_ENABLE (1 << 11 )

#define START_BYTE 0xA5
#define MAX_DATA_LEN 255
#define SERIAL_TIMEOUT 64

#define TIMER_DIV_1024  3


//////////////////////////////////////////////////////////
// COMMAND
typedef struct {
    uint8_t request_num;   // sequence / request number
    uint8_t command;    // command ( and response )code
    uint8_t length;
    uint8_t data[MAX_DATA_LEN];
    uint8_t chksum;
    uint8_t calculated_chksum;
} cr_packet;

// command/response byte
enum {
    CMD_PING = 0x00,
    CMD_GET_VERSION = 0x01,
    CMD_TIME = 0x02,
    CMD_ECHO = 0x03,


    CMD_DO_HTTP_GET = 0x04,
    CMD_DO_HTTP_POST = 0x05,
    CMD_DO_HTTP_PUT = 0x06,
    CMD_DO_HTTP_DELETE = 0x07,

    CMD_GET_HTTP_STATUS = 0x08,
    CMD_GET_HTTP_RESPONSE  = 0x09,


    CMD_SET_HTTP_PATH  = 0x0A,  // two part?
    CMD_SET_HTTP_QUERY  = 0x0B,  // three part?
    CMD_SET_HTTP_BODY  = 0x0C,  // N part?
    CMD_RESET_HTTP_REQUEST = 0x0D,
    CMD_SET_SERVER_NAME  = 0x0E,    // just do whole URL `http://the.server:port'



    RESP_OK = 0x80,    // completed request
    RESP_ACK = 0x81,   // acknowledge request received, results still pending
    RESP_ERR = 0xFF,   // errored out.

};


//////////////////////////////////////////////////////////
// SERIAL 
typedef enum
{
    AWAIT_START, // default state.  exit to command when current byte is A5
    GET_REQ_NUM, // read 1 byte and switch state to command
    GET_COMMAND, // read 1 byte and transition to length
    GET_LENGTH,  // read 1 byte and transition to data state
    GET_DATA,    // read N bytes. Transition to cksum cal
    GET_CHKSUM,  // calc cksum, send response, transition to WAIT state
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

static u8  get_byte( ) {
    // bit 4 == 1 waiting
    while (REG_SIOCNT & UART_RECEIVE_DATA_FLAG) {}
    return (u8) REG_SIODATA8;
}


uint8_t get_chksum( cr_packet *cp ) {
    uint8_t chksum = 0;
    //cp->chksum ^= START_BYTE;
    chksum ^= cp->request_num;   // not an actual check SUM but an XOR for now.
    chksum ^= cp->command;   // not an actual check SUM but an XOR for now.
    chksum ^= cp->length;
    for(int i=0; i < cp->length; ++i ) {
        chksum ^= cp->data[i];
    }
    return chksum;
}

typedef enum
{
    UART_READ_OK = 0,
    UART_READ_TIMEOUT,
    UART_READ_ERROR
} uart_read_result_t;



static void comm_timer_init(void)
{
    REG_TM3CNT_H = 0; // stop timer
    REG_TM3CNT_L = 0; // set reload value to zero for full count


    // start timer 3 with 1024 scaling
    REG_TM3CNT = TIMER_START | TIMER_DIV_1024;
}

static uint16_t ms_to_ticks(uint16_t ms)
{
    return ms * 16;
}





// my way, no good?
bool read_response(cr_packet *cp, int timeout_ms  )
{
    int16_t start = REG_TM3CNT_L;
    uint16_t timeout_ticks = ms_to_ticks( timeout_ms );

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
                        state = GET_REQ_NUM;
                    }
                    start = REG_TM3CNT_L;
                    break;
                case GET_REQ_NUM:
                    cp->request_num = b;
                    state = GET_COMMAND;
                    start = REG_TM3CNT_L;
                    break;
                case GET_COMMAND:
                    cp->command = b;
                    state = GET_LENGTH;
                    start = REG_TM3CNT_L;
                    break;
                case GET_LENGTH:
                    cp->length = b;
                    current_byte = 0;
                    if( cp->length > 0 ){
                        state = GET_DATA;
                    } else {
                        state = GET_CHKSUM;
                    }
                    start = REG_TM3CNT_L;

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
                    start = REG_TM3CNT_L;
                    break;
                case GET_CHKSUM:
                    cp->chksum = b;
                    cp->calculated_chksum = get_chksum(cp);
                    iprintf("GOT CHKSUM %d %d\n", b, cp->calculated_chksum);
                    state = AWAIT_START;
                    return true;
            }
        }
        int16_t now = REG_TM3CNT_L;
        int16_t elapsed = now - start;
        if( elapsed > timeout_ticks ) {
            return false;
        }
    }
}


// send command 
void send_cp( cr_packet *cp ) {
    // send SYNC
    send_byte( START_BYTE );
    send_byte( cp->request_num );
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

    uint8_t req_num = 0;  // used to keep track of multiple requests 

    while (1) {
        VBlankIntrWait();
        scanKeys(); // 
        u16 down = keysDown();
        if( down & KEY_A ) {
            ///////////////////////////////////////////////////////
            // HTTP GET ///////////////////////////////////////////
            // curl -X GET "http://localhost:5364/board?table=hastings"
            // send server:port 
            cr_packet cp = { req_num, CMD_SET_SERVER_NAME, 0, "greggallardo.com:5364", 0,0  };
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );
            cr_packet rp;

            read_response(&rp, 100 );
            iprintf("A1 GOT %d %d %d\n", rp.command,  req_num, rp.request_num );
            if( rp.command != RESP_OK ) {
            }

            // send path ( appended to server:port/  in the ESP32 )
            cp.command = CMD_SET_HTTP_PATH;
            strcpy( (char*) cp.data, "board");
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            
            iprintf("A2 GOT %d %d %d\n", rp.command,  req_num, rp.request_num );

            
            // send URL query (gets added to "?" in the ESP32 )
            cp.command = CMD_SET_HTTP_QUERY;
            strcpy( (char*) cp.data, "table=hastings");
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            
            iprintf("A3 GOT %d %d %d\n", rp.command,  req_num, rp.request_num );


            // tell ESP32 to perform the HTTP GET
            cp.command =  CMD_DO_HTTP_GET;
            cp.data[0] = 0;
            cp.length = 0;
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            
            iprintf("A4 GOT %d %d %d\n", rp.command,  req_num, rp.request_num );


            //  loop while reading response. Expect `RESP_OK` ( real code should also
            //  break the loop on error or if things take too long )
            while( true ) {
                cp.command =  CMD_GET_HTTP_RESPONSE;
                cp.data[0] = 0;
                cp.length = 0;
                cp.chksum = get_chksum( &cp );
                send_cp( &cp );
                read_response(&rp, 1000 );            
                iprintf("A5 GOT %d %d %d\n", rp.command,  req_num, rp.length );
                if( rp.command == RESP_OK ) {
                    break;
                }
            }
            iprintf( rp.data );

            // increase request.
            req_num++;

        }
        if( down & KEY_B ) {
            ///////////////////////////////////////////////////////
            // HTTP POST //////////////////////////////////////////
            // curl -X POST "http://localhost:5364/joingame?table=hastings" -d $'gorm\nW\n'

            // tell ESP32 to set its server:port  (only actually needed once if you don't change it)
            cr_packet cp = { req_num, CMD_SET_SERVER_NAME, 0, "greggallardo.com:5364", 0,0  };
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );
            cr_packet rp;

            read_response(&rp, 100 );            
            iprintf("B1 GOT %d %d %d\n", rp.command,  req_num, rp.request_num );

            
            // send path ( appended to server:port/  in the ESP32 )
            cp.command = CMD_SET_HTTP_PATH;
            strcpy( (char*) cp.data, "joingame");
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            
            iprintf("B2 GOT %d %d %d\n", rp.command,  req_num, rp.request_num );

            
            // send URL query (gets added to "?" in the ESP32 )
            cp.command = CMD_SET_HTTP_QUERY;
            strcpy( (char*) cp.data, "table=hastings");
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            
            iprintf("B3 GOT %d %d %d\n", rp.command,  req_num, rp.request_num );



            // send data to be added to POST 
            cp.command = CMD_SET_HTTP_BODY;
            strcpy( (char*) cp.data, "gormalkin\nW\n");
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            
            iprintf("B4 GOT %d %d %d\n", rp.command,  req_num, rp.request_num );


            // tell ESP32 to perform the HTTP POST
            cp.command = CMD_DO_HTTP_POST;
            cp.data[0] = 0;
            cp.length = 0;
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            
            iprintf("B5 GOT %d %d %d\n", rp.command,  req_num, rp.request_num );

            //  loop while reading response. Expect `RESP_OK` ( real code should also
            //  break the loop on error or if things take too long )
            while( true ) {
                cp.command =  CMD_GET_HTTP_RESPONSE;
                cp.data[0] = 0;
                cp.length = 0;
                cp.chksum = get_chksum( &cp );
                send_cp( &cp );
                read_response(&rp, 1000 );            
                iprintf("B6 GOT %d %d %d\n", rp.command,  req_num, rp.length );
                if( rp.command == RESP_OK ) {
                    break;
                }
            }
            iprintf( rp.data );

            

            req_num++;



        }
        if( down & KEY_R ) {

            cr_packet cp = { req_num, CMD_SET_SERVER_NAME, 0, "greggallardo.com:5364", 0,0  };            
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );
            cr_packet rp;

            read_response(&rp, 100 );            

            cp.command = CMD_SET_HTTP_PATH;
            strcpy( (char*) cp.data, "move");
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            


            cp.command = CMD_SET_HTTP_QUERY;
            strcpy( (char*) cp.data, "table=hastings");
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            

            cp.command = CMD_SET_HTTP_BODY;
            strcpy( (char*) cp.data, "e2e4");
            cp.length = strlen( (char*) cp.data );
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            


            cp.command = CMD_DO_HTTP_POST;
            cp.data[0] = 0;
            cp.length = 0;
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );

            read_response(&rp, 100 );            

            req_num++;


            req_num++;
        }

        // Non-HTTP test.
        if( down & KEY_L ) {
            cr_packet cp = { req_num, CMD_ECHO, 90, { 0,1,2,3,4,5,6,7,8,9, 0,1,2,3,4,5,6,7,8,9, 0,1,2,3,4,5,6,7,8,9, 0,1,2,3,4,5,6,7,8,9, 0,1,2,3,4,5,6,7,8,9, 0,1,2,3,4,5,6,7,8,9, 0,1,2,3,4,5,6,7,8,9, 0,1,2,3,4,5,6,7,8,9, 0,1,2,3,4,5,6,7,8,9, 0,1,2,3,4,5,6,7,8,9 }, 0  };            
            cp.chksum = get_chksum( &cp );
            send_cp( &cp );
            cr_packet rp;
            read_response(&rp, 100 );            
            iprintf("GOT  %d %d %d\n", rp.command,  req_num, rp.request_num );
            req_num++;
        }
    }
}


