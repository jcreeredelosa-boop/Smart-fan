
#include "app_hw.h"

#include <stdlib.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "driver/gpio.h"

#define BTN_CNT 3
#define TAG "app.btn"

// LED GPIO pins
#define APP_LED_LOW  GPIO_NUM_25
#define APP_LED_HIGH GPIO_NUM_26

// Active-low relays:
// 0 = ON
// 1 = OFF

static appbtn_fan_changed_f fan_cb = NULL;

static btn_map_t btn_map[BTN_CNT] = {
    {APP_BTN0, GPIO_NUM_NC, 0},
    {APP_BTN1, APP_RELAY1, 33},
    {APP_BTN2, APP_RELAY2, 100}
};

static uint8_t active_btn_idx = 0;
static QueueHandle_t idx_queue = NULL;
static TickType_t next = 0;


// Update LED indicators
static void update_leds(uint8_t idx)
{
    // LOW: Green ON, Red OFF
    // HIGH: Green OFF, Red ON
    // OFF: Both OFF

    gpio_set_level(APP_LED_LOW, idx == 1 ? 1 : 0);
    gpio_set_level(APP_LED_HIGH, idx == 2 ? 1 : 0);

    ESP_LOGI(TAG, "LED state: %s",
             idx == 1 ? "LOW - GREEN ON" :
             idx == 2 ? "HIGH - RED ON" :
                        "OFF - BOTH OFF");
}


// Button interrupt handler
static void IRAM_ATTR button_handler(void *arg)
{
    TickType_t now = xTaskGetTickCountFromISR();

    if (next > now)
        return;

    next = now + pdMS_TO_TICKS(100);

    uint8_t idx = (uint8_t)(uintptr_t)arg;

    BaseType_t higher_priority_task_woken = pdFALSE;

    xQueueSendToBackFromISR(
        idx_queue,
        &idx,
        &higher_priority_task_woken
    );

    if (higher_priority_task_woken)
        portYIELD_FROM_ISR();
}


// Relay control task
static void set_relay_task(void *arg)
{
    uint8_t idx;

    while (1)
    {
        if (xQueueReceive(idx_queue, &idx, portMAX_DELAY))
        {
            if (idx >= BTN_CNT || idx == active_btn_idx)
                continue;

            ESP_LOGI(TAG, "Changing fan state to %d",
                     btn_map[idx].val);

            // Turn both relays OFF
            gpio_set_level(APP_RELAY1, 1);
            gpio_set_level(APP_RELAY2, 1);

            // Turn both LEDs OFF
            update_leds(0);

            // Break-before-make delay
            vTaskDelay(pdMS_TO_TICKS(250));

            // Activate selected relay
            if (idx == 1)
            {
                gpio_set_level(APP_RELAY1, 0);
            }
            else if (idx == 2)
            {
                gpio_set_level(APP_RELAY2, 0);
            }

            // Update corresponding LED
            update_leds(idx);

            // Save current state
            active_btn_idx = idx;

            ESP_LOGI(TAG, "LOW relay GPIO32: %d",
                     gpio_get_level(APP_RELAY1));

            ESP_LOGI(TAG, "HIGH relay GPIO33: %d",
                     gpio_get_level(APP_RELAY2));

            ESP_LOGI(TAG, "GREEN LED GPIO25: %d",
                     gpio_get_level(APP_LED_LOW));

            ESP_LOGI(TAG, "RED LED GPIO26: %d",
                     gpio_get_level(APP_LED_HIGH));

            // Notify application
            if (fan_cb != NULL)
            {
                fan_cb(btn_map[idx].val);
            }
        }
    }
}


// Initialize hardware
void apphw_init(appbtn_fan_changed_f f)
{
    fan_cb = f;

    // Create state-change queue
    idx_queue = xQueueCreate(5, sizeof(uint8_t));

    if (idx_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create relay queue");
        return;
    }

    gpio_config_t io_conf = {0};

    // Configure relay outputs
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask =
        (1ULL << APP_RELAY1) |
        (1ULL << APP_RELAY2);
    io_conf.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&io_conf);

    // Initially turn relays OFF
    gpio_set_level(APP_RELAY1, 1);
    gpio_set_level(APP_RELAY2, 1);

    // Configure LED outputs
    io_conf = (gpio_config_t){0};
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask =
        (1ULL << APP_LED_LOW) |
        (1ULL << APP_LED_HIGH);
    io_conf.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&io_conf);

    // Initially turn LEDs OFF
    update_leds(0);

    // Configure button inputs
    io_conf = (gpio_config_t){0};
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask =
        (1ULL << APP_BTN0) |
        (1ULL << APP_BTN1) |
        (1ULL << APP_BTN2);
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    gpio_config(&io_conf);

    // Create relay control task
    BaseType_t task_result = xTaskCreate(
        set_relay_task,
        "relay_task",
        2048,
        NULL,
        10,
        NULL
    );

    if (task_result != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create relay task");
        return;
    }

    // Install GPIO interrupt service
    esp_err_t err = gpio_install_isr_service(0);

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE(TAG, "Failed to install ISR service");
        return;
    }

    // Register button interrupts
    for (int i = 0; i < BTN_CNT; i++)
    {
        gpio_isr_handler_add(
            btn_map[i].btn_pin,
            button_handler,
            (void *)(uintptr_t)i
        );
    }

    ESP_LOGI(TAG, "Hardware initialized successfully");
}


// Get current fan state
uint8_t apphw_get_state(void)
{
    return btn_map[active_btn_idx].val;
}


// Change fan state externally (Home Assistant / MQTT)
void apphw_set_state(uint8_t val)
{
    uint8_t idx;

    if (val == 0)
    {
        idx = 0;
    }
    else if (val <= 33)
    {
        idx = 1;
    }
    else
    {
        idx = 2;
    }

    if (idx_queue != NULL)
    {
        xQueueSendToBack(idx_queue, &idx, 0);
    }
}
