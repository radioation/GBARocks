#include <stdio.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#include "http_client.h"
#include "wifi.h"
#include "GBA_comm.h"


static QueueHandle_t request_queue;
static QueueHandle_t response_queue;

static cr_response responses[ 256 ];

static void gba_comm_task(void *arg)
{
    cr_packet request;
    cr_response response;
                            
    char buffer[ 128 ];

    for (;;) {
        if (gba_comm_read_packet(&request, 100)) {

            printf( "GBA req: req_num=%u cmd=%d\n", request.request_num, request.command);

            if( request.calculated_chksum == request.chksum ) {
                switch( request.command ) {
                    case CMD_PING:
                        {
                            printf("** GOT PING CMD\n");
                            cr_packet resp = {.data = "PONG"};
                            resp.request_num = request.request_num;
                            resp.command = RESP_OK;
                            resp.length = 4;
                            resp.chksum = gba_comm_get_chksum(&resp);
                            gba_comm_send_packet(&resp);
                        }
                        break;
                    case CMD_GET_VERSION:
                        {

                            printf("** GOT VERSION CMD\n");
                            cr_packet resp = {.data = "0.1"};
                            resp.request_num = request.request_num;
                            resp.command = RESP_OK;
                            resp.length = 3;
                            resp.chksum = gba_comm_get_chksum(&resp);
                            gba_comm_send_packet(&resp);
                        }
                        break;
                    case CMD_TIME:
                        {
                        }
                        break;
                    case CMD_ECHO:
                        {

                            printf("** GOT ECHO CMD\n");
                            cr_packet resp;
                            resp.request_num = request.request_num;
                            resp.command = RESP_OK;
                            resp.length = request.length;
                            memcpy(resp.data, request.data, request.length);
                            resp.chksum = gba_comm_get_chksum(&resp);
                            printf("**   RESPONSE len %d cksum %d\n", resp.length, resp.chksum);
                            gba_comm_send_packet(&resp);
                        }
                        break;
                    case CMD_DO_HTTP_GET:
                        {
                            printf("** GOT GET REQUEST\n");
                            //  Try to give the request to the network worker.
                            /// 0 timeout so we keep spinning
                            if (xQueueSend( request_queue, &request, 0  ) == pdTRUE) {

                                printf("  QUEUED GET REQUEST %d\n", request.request_num);
                                cr_response new_resp = {0};
                                responses[ request.request_num ] = new_resp;
                                // send acknowledgement.
                                cr_packet resp = { request.request_num, RESP_ACK, 0, {}, 0,0 };
                                resp.chksum = gba_comm_get_chksum(&resp);
                                gba_comm_send_packet(&resp);


                            } else {
                                printf("  FAILED TO QUEUE GET REQUEST\n");
                                // we're having  problems
                                cr_packet resp = { request.request_num, RESP_ERR, 0, {}, 0, 0 };
                                resp.chksum = gba_comm_get_chksum(&resp);
                                gba_comm_send_packet(&resp);

                            }

                        }
                        break;
                    case CMD_DO_HTTP_POST:
                        {
                            printf("** GOT POST REQUEST\n");
                            //  Try to give the request to the network worker.
                            /// 0 timeout so we keep spinning
                            if (xQueueSend( request_queue, &request, 0  ) == pdTRUE) {

                                printf("  QUEUED POST REQUEST %d\n", request.request_num);

                                cr_response new_resp = {0};
                                responses[ request.request_num ] = new_resp;
                                // send acknowledgement.
                                cr_packet resp = { request.request_num, RESP_ACK, 0, {}, 0,0 };
                                resp.chksum = gba_comm_get_chksum(&resp);
                                gba_comm_send_packet(&resp);


                            } else {
                                printf("  FAILED TO QUEUE POST REQUEST\n");
                                // we're having  problems
                                cr_packet resp = { request.request_num, RESP_ERR, 0, {}, 0, 0 };
                                resp.chksum = gba_comm_get_chksum(&resp);
                                gba_comm_send_packet(&resp);
                            }
                        }
                        break;
                    case CMD_DO_HTTP_PUT:
                        {
                            printf("** GOT PUT REQUEST (not implemented)\n");
                        }
                        break;
                    case CMD_DO_HTTP_DELETE:
                        {
                            printf("** GOT DELETE REQUEST (not implemented)\n");
                        }
                        break;


                    case CMD_GET_HTTP_STATUS:
                        {
                            printf("** GOT GET STATUS REQUEST\n");
                            // check status or HTTP Result in quue
                            response = responses[ request.request_num ];
                            if( response.response_code == 0 ) {
                                // send acknowledgement.
                                cr_packet resp = { request.request_num, RESP_ACK, 0, {}, 0,0 };
                                resp.chksum = gba_comm_get_chksum(&resp);
                                gba_comm_send_packet(&resp);
                            }  else {
                                // send acknowledgement.
                                printf( "STATUS CODE %d\n", response.response_code );
                                cr_packet resp = { request.request_num, RESP_OK, 2, {}, 0,0 };
                                memcpy(resp.data, &response.response_code, 2);
                                resp.chksum = gba_comm_get_chksum(&resp);
                                gba_comm_send_packet(&resp);
                            }

                        }
                        break;
                    case CMD_GET_HTTP_RESPONSE:
                        {
                            printf("** GOT GET RESPONSE REQUEST\n");
                            // check status or HTTP Result in quue
                            // check status or HTTP Result in quue
                            response = responses[ request.request_num ];
                            printf( "  >>> req  %d  CODE %d\n", request.request_num,  response.response_code );
                            if( response.response_code == 0 ) {
                                // send acknowledgement.
                                printf( "SEND ACK FOR  %d\n", response.response_code );
                                cr_packet resp = { request.request_num, RESP_ACK, 0, {}, 0,0 };
                                resp.chksum = gba_comm_get_chksum(&resp);
                                gba_comm_send_packet(&resp);
                            }  else {
                                // send acknowledgement.
                                printf( "SEND OK FOR CODE %d\n", response.response_code );
                                printf( "  >>> response. data: %s\n", response.data );
                                cr_packet resp = { request.request_num, RESP_OK, response.length, {}, 0,0 };
                                strncpy( (char*) resp.data, (char*) response.data, response.length ); 
                                printf( "  >>> resp data: %s\n", resp.data );
                                resp.chksum = gba_comm_get_chksum(&resp);
                                gba_comm_send_packet(&resp);
                            }

                        }
                        break;
                    case CMD_SET_HTTP_PATH:
                        {
                            printf("** GOT SET HTTP PATH\n");
                            memcpy(buffer, request.data, request.length);
                            buffer[request.length] = 0;
                            http_set_path( buffer ); 

                            cr_packet resp = { request.request_num, RESP_OK, 0, {}, 0, 0 };
                            resp.chksum = gba_comm_get_chksum(&resp);
                            printf("**   RESPONSE len %d cksum %d\n", resp.length, resp.chksum);
                            gba_comm_send_packet(&resp);
                        }
                        break;
                    case CMD_SET_HTTP_QUERY:
                        {
                            printf("** GOT SET HTTP QUERY\n");
                            memcpy(buffer, request.data, request.length);
                            buffer[request.length] = 0;
                            http_set_query( buffer ); 

                            cr_packet resp = { request.request_num, RESP_OK, 0, {}, 0, 0 };
                            resp.chksum = gba_comm_get_chksum(&resp);
                            printf("**   RESPONSE len %d cksum %d\n", resp.length, resp.chksum);
                            gba_comm_send_packet(&resp);
                        }
                        break;
                    case CMD_SET_HTTP_BODY:
                        {
                            printf("** GOT SET HTTP BODY\n");
                            memcpy(buffer, request.data, request.length);
                            buffer[request.length] = 0;
                            http_set_body( buffer ); 

                            cr_packet resp = { request.request_num, RESP_OK, 0, {}, 0, 0 };
                            resp.chksum = gba_comm_get_chksum(&resp);
                            printf("**   RESPONSE len %d cksum %d\n", resp.length, resp.chksum);
                            gba_comm_send_packet(&resp);
                        }
                        break;
                    case CMD_RESET_HTTP_REQUEST:
                        {
                            printf("** GOT RESET HTTP REQUEST\n");
                            // check status or HTTP Result in quue
                            http_reset_request();
                            cr_packet resp = { request.request_num, RESP_OK, 0, {}, 0, 0 };
                            resp.chksum = gba_comm_get_chksum(&resp);
                            printf("**   RESPONSE len %d cksum %d\n", resp.length, resp.chksum);
                            gba_comm_send_packet(&resp);

                        }
                        break;
                    case CMD_SET_SERVER_NAME:
                        {
                            printf("** GOT SET SERVER NAM\n");
                            memcpy(buffer, request.data, request.length);
                            buffer[request.length] = 0;
                            http_set_server_name( buffer ); 

                            cr_packet resp = { request.request_num, RESP_OK, 0, {}, 0, 0 };
                            resp.chksum = gba_comm_get_chksum(&resp);
                            printf("**   RESPONSE len %d cksum %d\n", resp.length, resp.chksum);
                            gba_comm_send_packet(&resp);
                        }
                        break;

                    default: 
                        {
                            printf( "Unknown command: %02X\n", request.command );
                        }
                        break;
                }
            }
        }


        // look for results and store as reult
        /// 0 timeout so we keep spinning
        while (xQueueReceive( response_queue, &response, 0 ) == pdTRUE) {

            //save_response(&response);
            printf( "gba_comm_task got response %d \n", response.request_num);
            printf( ">>>>>>>>> response_code  %d \n", response.response_code);
            responses[ response.request_num ] = response;
        }
    }
}


static void network_task(void *arg)
{
    cr_packet request;
    cr_response response;

    for (;;) {
        // wait for work
        if (xQueueReceive(
                    request_queue,
                    &request,
                    portMAX_DELAY // wait time
                    ) == pdTRUE) {

            printf( "network_task got request: seq=%u\n", request.request_num);


            memset(&response, 0, sizeof(response));
            response.request_num = request.request_num;


            // BLOCKING!!! waiting for response.
            switch( request.command ) {
                case CMD_DO_HTTP_GET:
                    {
                        //response.success = perform_server_request( &request, &response);
                        http_get_request( &response );
                    }
                    break;
                case CMD_DO_HTTP_POST:
                    {
                        http_post_request( &response );
                    }
                    break;
                case CMD_DO_HTTP_PUT:
                    {
                    }
                    break;
                case CMD_DO_HTTP_DELETE:
                    {
                    }
                    break;
                default:
                    break;
            };

            //  send results to UART queue
            printf("network_task QUEUE HTTP RESPONSE %u\n", response.request_num);
            xQueueSend(
                    response_queue,
                    &response,
                    portMAX_DELAY
                    );
        }
    }
}



void app_main(void)
{
    // Initialize NVS flash memory
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // uart
    gba_comm_init();    
    // wifi
    wifi_init();


    request_queue =
        xQueueCreate(
                4,
                sizeof(cr_packet)
                );

    response_queue =
        xQueueCreate(
                4,
                sizeof(cr_response)
                );


    if (!request_queue || !response_queue) {
        printf("Failed to create queues\n");
        return;
    }


    xTaskCreate(
            gba_comm_task,
            "gba_comm",
            4096,
            NULL,
            10,
            NULL
            );


    xTaskCreate(
            network_task,
            "network",
            8192,
            NULL,
            5,
            NULL
            );




}
