#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "app_wifi.h"
#include "app_hw.h"
#include "app_mqtt.h"

#define TAG "app"

static void handle_wifi_connect(void)
{
    ESP_LOGI(TAG, "Wi-Fi connected. Starting MQTT...");
    appmqtt_start();
}

static void handle_wifi_failed(void)
{
    ESP_LOGE(TAG, "Wi-Fi connection failed");
}

void app_main(void)
{
    appmqtt_init(apphw_set_state);

    apphw_init(appmqtt_publish);

    connect_wifi_params_t cbs = {
        .on_connected = handle_wifi_connect,
        .on_failed = handle_wifi_failed
    };

    appwifi_connect(cbs);
}