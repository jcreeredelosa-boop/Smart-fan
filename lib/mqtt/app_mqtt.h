#ifndef app_mqtt_h_
#define app_mqtt_h_

#include <stdint.h>

typedef void (*mqtt_fan_state_changed_f)(uint8_t);

void appmqtt_init(mqtt_fan_state_changed_f callback);
void appmqtt_start(void);
void appmqtt_publish(uint8_t state);

#endif