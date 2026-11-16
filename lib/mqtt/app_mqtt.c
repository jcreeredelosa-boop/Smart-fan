    #include "app_mqtt.h"

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <stdbool.h>

    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"

    #include "esp_log.h"
    #include "mqtt_client.h"
    #include "esp_event.h"

    // MQTT configuration
    #define TAG "app.mqtt"

    #define MQTT_BROKER_URI "mqtt://172.20.10.2:1883"
    #define MQTT_USERNAME   "smartfan_mqtt"

    // Put your actual Mosquitto password here.
    // Do not share your password publicly.
    #define MQTT_PASSWORD   "redelosa29"

    // MQTT topics
    #define MQTT_TOPIC_SET   "smartfan/set"
    #define MQTT_TOPIC_STATE "smartfan/state"

    // MQTT client variables
    static esp_mqtt_client_handle_t mqtt_client = NULL;
    static mqtt_fan_state_changed_f fan_callback = NULL;
    static bool mqtt_connected = false;


    // Convert numeric fan state into text
    static const char *state_to_text(uint8_t state)
    {
        if (state == 0)
        {
            return "OFF";
        }

        if (state <= 33)
        {
            return "LOW";
        }

        return "HIGH";
    }


    // Handle MQTT events
    static void mqtt_event_handler(
        void *handler_args,
        esp_event_base_t base,
        int32_t event_id,
        void *event_data)
    {
        esp_mqtt_event_handle_t event =
            (esp_mqtt_event_handle_t)event_data;

        switch ((esp_mqtt_event_id_t)event_id)
        {
            case MQTT_EVENT_CONNECTED:
            {
                ESP_LOGI(TAG, "Connected to Home Assistant MQTT broker");

                mqtt_connected = true;

                esp_mqtt_client_subscribe(
                    mqtt_client,
                    MQTT_TOPIC_SET,
                    1
                );

                ESP_LOGI(TAG, "Subscribed to: %s", MQTT_TOPIC_SET);
                break;
            }

            case MQTT_EVENT_DISCONNECTED:
            {
                ESP_LOGW(TAG, "Disconnected from MQTT broker");

                mqtt_connected = false;
                break;
            }

            case MQTT_EVENT_DATA:
            {
                char topic[128] = {0};
                char message[32] = {0};

                if (event->topic_len >= sizeof(topic) ||
                    event->data_len >= sizeof(message))
                {
                    ESP_LOGW(TAG, "MQTT message too long");
                    break;
                }

                memcpy(topic, event->topic, event->topic_len);
                memcpy(message, event->data, event->data_len);

                ESP_LOGI(TAG, "Received topic: %s", topic);
                ESP_LOGI(TAG, "Received message: %s", message);

                if (strcmp(topic, MQTT_TOPIC_SET) == 0)
                {
                    if (strcmp(message, "OFF") == 0)
                    {
                        if (fan_callback != NULL)
                        {
                            fan_callback(0);
                        }
                    }
                    else if (strcmp(message, "LOW") == 0)
                    {
                        if (fan_callback != NULL)
                        {
                            fan_callback(33);
                        }
                    }
                    else if (strcmp(message, "HIGH") == 0)
                    {
                        if (fan_callback != NULL)
                        {
                            fan_callback(100);
                        }
                    }
                    else
                    {
                        ESP_LOGW(TAG, "Unknown fan command: %s", message);
                    }
                }

                break;
            }

            case MQTT_EVENT_ERROR:
            {
                ESP_LOGE(TAG, "MQTT connection error");
                break;
            }

            default:
            {
                break;
            }
        }
    }


    // Initialize MQTT callback
    void appmqtt_init(mqtt_fan_state_changed_f callback)
    {
        fan_callback = callback;
    }


    // Start MQTT connection
    void appmqtt_start(void)
    {
        esp_mqtt_client_config_t mqtt_config = {
            .uri = MQTT_BROKER_URI,
            .username = MQTT_USERNAME,
            .password = MQTT_PASSWORD
        };

        mqtt_client = esp_mqtt_client_init(&mqtt_config);

        if (mqtt_client == NULL)
        {
            ESP_LOGE(TAG, "Failed to initialize MQTT client");
            return;
        }

        esp_mqtt_client_register_event(
            mqtt_client,
            ESP_EVENT_ANY_ID,
            mqtt_event_handler,
            NULL
        );

        esp_mqtt_client_start(mqtt_client);

        ESP_LOGI(TAG, "Starting MQTT client...");
    }


    // Publish current fan state
    void appmqtt_publish(uint8_t state)
    {
        if (mqtt_client == NULL || !mqtt_connected)
        {
            ESP_LOGW(TAG, "MQTT is not connected. State not published.");
            return;
        }

        const char *message = state_to_text(state);

        esp_mqtt_client_publish(
            mqtt_client,
            MQTT_TOPIC_STATE,
            message,
            0,
            1,
            0
        );

        ESP_LOGI(TAG, "Published fan state: %s", message);
    }