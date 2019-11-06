#pragma once

#include<MqttBaseLight.hpp>

class MqttMOSFETLight : public MqttBaseLight
{
public:
    MqttMOSFETLight(uint8_t pin_a, MqttController &mqtt, const char *mqtt_topic_control_state_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a);
    
    const uint8_t pin;
    
    void setLevel();
    void begin(const uint8_t level);
};