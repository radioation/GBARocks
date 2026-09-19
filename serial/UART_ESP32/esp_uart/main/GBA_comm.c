
#include "GBA_comm.h"

void gba_comm_init()
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

uint8_t gba_comm_get_chksum(cr_packet *cp)
{
    uint8_t chksum = 0;
    // cp->chksum ^= START_BYTE;
    chksum ^= cp->request_num; // not an actual checksum. XOR for now.
    chksum ^= cp->command; // not an actual checksum. XOR for now.
    chksum ^= cp->length;
    for (int i = 0; i < cp->length; ++i)
    {
        chksum ^= cp->data[i];
    }
    return chksum;
}

bool gba_comm_read_packet(cr_packet *cp, int timeout_ms )
{
    serial_state state = AWAIT_START;
    int current_byte = 0;
    TickType_t start = xTaskGetTickCount();
    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout_ms);
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
                        state = GET_REQ_NUM;
                    }
                    start = xTaskGetTickCount(); // reset.
                    break;
                case GET_REQ_NUM:
                    //printf("GET REQUEST NUMBER %d\n",b);
                    cp->request_num = b;
                    state = GET_COMMAND;
                    start = xTaskGetTickCount(); // reset.
                    break;
                case GET_COMMAND:
                    //printf("GET CMD %d\n",b);
                    cp->command = b;
                    state = GET_LENGTH;
                    start = xTaskGetTickCount(); // reset.
                    break;
                case GET_LENGTH:
                    //printf("GET LEN %d\n", b);
                    cp->length = b;
                    current_byte = 0;
                    if( cp->length > 0 ){
                        state = GET_DATA;
                    } else {
                        state = GET_CHKSUM;
                    }
                    start = xTaskGetTickCount(); // reset.

                    break;
                case GET_DATA:
                    //printf("GET DATA %d %d\n", current_byte, b);
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
                    start = xTaskGetTickCount(); // reset.
                    break;
                case GET_CHKSUM:
                    //printf("GET CHKSUM");
                    cp->chksum = b;
                    cp->calculated_chksum = gba_comm_get_chksum(cp);
                    //printf("GET CHKSUM %d %d\n", b, cp->calculated_chksum);
                    state = AWAIT_START;
                    return true;
            }
        }

        TickType_t now = xTaskGetTickCount();
        TickType_t elapsed = now - start;
        if( elapsed > timeout_ticks ) {
            return false;
        } 
    }
}

void gba_comm_send_packet(cr_packet *cp)
{
    // send SYNC
    const uint8_t startByte = START_BYTE;
    uart_write_bytes(UART_NUM_1, &startByte, 1);
    uart_write_bytes(UART_NUM_1, &(cp->request_num), 1);
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
