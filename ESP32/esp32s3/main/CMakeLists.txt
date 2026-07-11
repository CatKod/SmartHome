idf_component_register(SRCS "main.c"
                            "uart_handler.c"
                            "uart_protocol.c"
                            "wifi_manager.c"
                            "mqtt_manager.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_uart
                                  esp_wifi
                                  esp_netif
                                  esp_event
                                  nvs_flash)
