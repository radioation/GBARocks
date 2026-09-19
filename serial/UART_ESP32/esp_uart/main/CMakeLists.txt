idf_component_register(SRCS "GBA_comm.c" "http_client.c" "wifi.c" "main.c"
                    INCLUDE_DIRS "."
                       REQUIRES esp_wifi esp_driver_gpio esp_driver_uart esp_event esp_netif nvs_flash esp_http_client)
