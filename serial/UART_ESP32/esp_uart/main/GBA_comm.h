#ifndef _GBA_COMM_H_
#define _GBA_COMM_H_

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "driver/uart.h"
#include "driver/gpio.h"

// GBA
/*
1	VCC xxx
2	SO  TX    -> GPIO 22
3	SI  RX    <- GPIO 21
4	SD  RTS
5	SC  CTS
6	GND GND   == GND

GBA TX -> ESP32 RX
GBA RX <- ESP32 TX
GND    --- GND

*/
#define START_BYTE 0xA5
#define MAX_DATA_LEN 255
#define SERIAL_TIMEOUT 64

#define TX_PIN GPIO_NUM_21
#define RX_PIN GPIO_NUM_22
#define BUFFER_SIZE 256

//////////////////////////////////////////////////////////
// COMMAND/REQUEST PACKET
typedef struct
{
  uint8_t request_num;   // sequence / request number (should be ok for synchronous GBA-to-ESP communication. could be OK for 
  uint8_t command;       // command (and response) code
  uint8_t length;
  uint8_t data[MAX_DATA_LEN];
  uint8_t chksum; // chksum from GBA
  uint8_t calculated_chksum;
} cr_packet;

// command/response codes
enum
{
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


typedef enum {
    CMD_RESULT_EMPTY,
    CMD_RESULT_PENDING,
    CMD_RESULT_READY
} command_result_state;


typedef struct {
    uint8_t request_num;
    int16_t response_code; // for now sub in HTTP CODES, but could be anything
    uint8_t length;
    uint8_t data[MAX_DATA_LEN];
    uint8_t data_index;  // for ESP-IDF HTTP_EVENT_ON_DATA handling.
} cr_response;


typedef struct {
    command_result_state state;
    uint8_t request_num;
    cr_response response;
} current_result;


//////////////////////////////////////////////////////////
// Serial States
typedef enum
{
  AWAIT_START, // default state.  exit to command when current byte is 7F
  GET_REQ_NUM, // read 1 byte and switch state to command
  GET_COMMAND, // read 1 byte and switch state to length
  GET_LENGTH,  // read 1 byte (set N) then switch state to data 
  GET_DATA,    // read N bytes. Switch to chksum calculation.
  GET_CHKSUM,  // calc cksum, send response, switch to AWAIT_START state
} serial_state;

void gba_comm_init();

uint8_t gba_comm_get_chksum(cr_packet *cp);

bool gba_comm_read_packet(cr_packet *cp, int timeout_ms );
void gba_comm_send_packet(cr_packet *cp);



#endif // _GBA_COMM_H_
