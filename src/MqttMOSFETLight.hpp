#pragma once

#include<MqttBaseLight.hpp>
#include<Arduino.h>

class MqttMOSFETLight : public MqttBaseLight
{
public:
    MqttMOSFETLight(uint8_t pin_a, MqttController &mqtt, const char *mqtt_topic_control_state_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a, int8_t channel=-1, int8_t freq=-1, int8_t res=-1);
    
    uint8_t pin;
    
    void commit();
    void begin(const uint8_t brightness);
};
