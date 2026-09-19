#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "http_client.h"

const char *TAG = "HTTP_CLIENT";

static char server[128];


static http_request_state request_state;


// Event handler to process the HTTP response
esp_err_t http_event_handler(esp_http_client_event_t *evt) 
{

    cr_response *ctx = (cr_response *)evt->user_data;
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER: %s = %s", evt->header_key, evt->header_value);
            if (ctx) {
                ctx->response_code = esp_http_client_get_status_code(evt->client);
              }
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            // Print the response body chunk received from the server
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s\n", evt->data_len, (char*)evt->data);
            }
            if( ctx && evt->data_len > 0 ) {
                if( ctx-> data_index + evt->data_len < MAX_DATA_LEN -1) {
                    memcpy( ctx->data + ctx->data_index, evt->data, evt->data_len );
                    ctx->data_index += evt->data_len;
                    ctx->data[ ctx->data_index] = '\0'; // null
                    ctx->length = evt->data_len;
                } else {
                    ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA OVERFLOW!!");
                }
            }
            
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        default:
            break;
    }
    return ESP_OK;
}

void http_set_server_name( char* svr ) 
{
    strcpy( server, svr );
}


void http_set_path( char* path )
{
    strcpy( request_state.path, path );
}

void http_set_query( char* query )
{
    strcpy( request_state.query, query );
}

void http_set_body( char* body )
{
    strcpy( request_state.body, body );
}



void http_reset_request( )
{
    memset( &request_state, 0, sizeof(request_state));
}


void http_get_request( cr_response* response )
{
    printf("GET REQUEST\n");

    char url[512];
    memset( url, 0, sizeof( url ) );
    strcpy( url, "http://" );
    strcat( url, server );
    if( strlen( request_state.path ) > 0 ) {
        strcat(url, "/" );
        strcat(url, request_state.path );
    }
    if( strlen( request_state.query ) > 0 ) {
        strcat(url, "?" );
        strcat(url, request_state.query );
    }
    printf("GET REQUEST : %s\n", url);

    // Configure the client properties
    esp_http_client_config_t conf = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .user_data = response,
        .event_handler = http_event_handler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&conf);

    // REMEBER esp_http_client_perform() blocks by default.
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    // cleanup
    esp_http_client_cleanup(client);
}

void http_post_request( cr_response* response )
{
    // "http://localhost:5364/joingame?table=bangkok" -d $'radyo\nW\n'
    char url[512];
    strcpy( url, "http://" );
    strcat( url, server );
    if( strlen( request_state.path ) > 0 ) {
        strcat(url, "/" );
        strcat(url, request_state.path );
    }
    if( strlen( request_state.query ) > 0 ) {
        strcat(url, "?" );
        strcat(url, request_state.query );
    }

    esp_http_client_config_t config = {
        .url = url, 
        .method = HTTP_METHOD_POST,
        .user_data = response,
        .event_handler = http_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Set Content-Type header 
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");

    // addmove 
    esp_http_client_set_post_field(client, request_state.body, strlen(request_state.body));

    // REMEMBER esp_http_client_perform() blocks by default.
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST failed: %s", esp_err_to_name(err));
    }

    
    esp_http_client_cleanup(client);
}
