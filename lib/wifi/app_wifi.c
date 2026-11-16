
#include "app_wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define MAX_RETRY 10

static const char *TAG = "app_wifi";

static int retry_cnt = 0;
static connect_wifi_params_t m_params;

static void handle_wifi_connection(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data)
{
    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Connecting to Wi-Fi: %s", WIFI_SSID);
        esp_wifi_connect();
    }

    else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wifi_event_sta_disconnected_t *event =
            (wifi_event_sta_disconnected_t *)event_data;

        ESP_LOGW(TAG, "Wi-Fi disconnected. Reason: %d",
                 event->reason);

        if (retry_cnt < MAX_RETRY)
        {
            retry_cnt++;
            ESP_LOGI(TAG, "Retrying Wi-Fi connection (%d/%d)",
                     retry_cnt, MAX_RETRY);

            esp_wifi_connect();
        }
        else
        {
            ESP_LOGE(TAG, "Wi-Fi connection failed");

            if (m_params.on_failed)
            {
                m_params.on_failed();
            }
        }
    }

    else if (event_base == IP_EVENT &&
             event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *)event_data;

        retry_cnt = 0;

        ESP_LOGI(TAG, "Wi-Fi connected successfully!");
        ESP_LOGI(TAG, "IP Address: " IPSTR,
                 IP2STR(&event->ip_info.ip));

        if (m_params.on_connected)
        {
            m_params.on_connected();
        }
    }
}

void appwifi_connect(connect_wifi_params_t p)
{
    m_params = p;

    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGW(TAG, "Erasing NVS flash");

        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &handle_wifi_connection,
            NULL
        )
    );

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &handle_wifi_connection,
            NULL
        )
    );

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            }
        }
    };

    ESP_LOGI(TAG, "Using Wi-Fi SSID: %s", WIFI_SSID);

    ESP_ERROR_CHECK(
        esp_wifi_set_mode(WIFI_MODE_STA)
    );

    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config
        )
    );

    ESP_ERROR_CHECK(
        esp_wifi_set_ps(WIFI_PS_NONE)
    );

    ESP_ERROR_CHECK(
        esp_wifi_start()
    );
}
