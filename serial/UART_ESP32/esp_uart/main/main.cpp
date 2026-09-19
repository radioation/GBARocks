#include <stdio.h>
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
#define START_BYTE 0x7F
#define MAX_DATA_LEN 64
#define SERIAL_TIMEOUT 64

#define TX_PIN GPIO_NUM_21
#define RX_PIN GPIO_NUM_22
#define BUFFER_SIZE 256

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
  CMD_JOIN = 0x04,
  CMD_MOVE = 0x05,

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

void init()
{
  uart_config_t cfg = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };

  uart_driver_install(UART_NUM_1, BUFFER_SIZE, 0, 0, NULL, 0);

  uart_param_config(UART_NUM_1, &cfg);

  //Set UART pins(              TX: IO4, RX: IO5,    RTS: IO18,          CTS: IO19,       DTR: UNUSED, DSR: UNUSED)
  //uart_set_pin(UART_NUM_1, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_set_pin(UART_NUM_1, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

uint8_t get_chksum(cmd_packet *cp)
{
  uint8_t chksum = 0;
  // cp->chksum ^= START_BYTE;
  chksum ^= cp->command; // not an actual checksum. XOR for now.
  chksum ^= cp->length;
  for (int i = 0; i < cp->length; ++i)
  {
    chksum ^= cp->data[i];
  }
  return chksum;
}

bool read_command(cmd_packet *cp)
{
  SerialState state = AWAIT_START;
  int current_byte = 0;
  while (true)
  {
    uint8_t b;
    int count = uart_read_bytes(UART_NUM_1, &b, 1, pdMS_TO_TICKS(100));
    if (count)
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
        printf("GET CMD %d\n",b);
        cp->command = b;
        state = GET_LENGTH;
        break;
      case GET_LENGTH:
        printf("GET LEN %d\n", b);
        cp->length = b;
        current_byte = 0;
        if( cp->length > 0 ){
          state = GET_DATA;
        } else {
          state = GET_CHKSUM;
        }

        break;
      case GET_DATA:
        printf("GET DATA %d %d\n", current_byte, b);
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
        printf("GET CHKSUM");
        cp->chksum = b;
        cp->calculated_chksum = get_chksum(cp);
        printf("GET CHKSUM %d %d\n", b, cp->calculated_chksum);
        state = AWAIT_START;
        return true;
      }
    }
  }
}

void send_command(cmd_packet *cp)
{
  // send SYNC
  const uint8_t startByte = START_BYTE;
  uart_write_bytes(UART_NUM_1, &startByte, 1);
  //vTaskDelay(pdMS_TO_TICKS(10));
  uart_write_bytes(UART_NUM_1, &(cp->command), 1);
  //vTaskDelay(pdMS_TO_TICKS(10));
  uart_write_bytes(UART_NUM_1, &(cp->length), 1);
  //vTaskDelay(pdMS_TO_TICKS(10));
  // uart_write_bytes(UART_NUM_1, cp->djjata, cp->length);
  for (int i = 0; i < cp->length; ++i)
  {
    uart_write_bytes(UART_NUM_1, &(cp->data[i]), 1);
    //vTaskDelay(pdMS_TO_TICKS(10));
  }
    uart_write_bytes(UART_NUM_1, &(cp->chksum), 1);
  
}
void app_main(void)
{
  uint8_t b;

  init();
  int total_read = 0;
  // while( total_read < 20) {
  while (true)
  {
    cmd_packet cp;

    bool didRead = read_command(&cp);
    if (didRead)
    {
      if (cp.calculated_chksum == cp.chksum)
      {
        switch (cp.command)
        {
        case CMD_PING:
        {
          printf("** GOT PING CMD\n");
          cmd_packet resp = {.data = "PONG"};
          resp.command = RESP_PING;
          resp.length = 4;
          resp.chksum = get_chksum(&resp);
          printf("**   PING RESPONSE len %d cksum %d\n", resp.length, resp.chksum);
          send_command(&resp);
        }
        break;
        case CMD_GET_VERSION:
        {

          printf("** GOT VERSION CMD\n");
          cmd_packet resp = {.data = "0.1"};
          resp.command = RESP_VERSION;
          resp.length = 3;
          resp.chksum = get_chksum(&resp);
          printf("**   VERSION RESPONSE len %d cksum %d\n", resp.length, resp.chksum);
          send_command(&resp);
        }
        break;
        case CMD_TIME:
          break;
        case CMD_ECHO:
        {

          printf("** GOT ECHO CMD\n");
          cmd_packet resp;
          resp.command = RESP_ECHO;
          resp.length = cp.length;
          memcpy(resp.data, cp.data, cp.length);
          resp.chksum = get_chksum(&resp);
          printf("**   ECHO RESPONSE len %d cksum %d\n", resp.length, resp.chksum);
          send_command(&resp);
        }
        default:
          break;
          printf("bad command:  %02X \n", cp.command);
        }
      }
      else
      {
        printf("bad chksum: orig: %02X calc: %02X\n", cp.chksum, cp.calculated_chksum);
      }
    }
    else
    {
      // nothing for now
    }
  }
}
