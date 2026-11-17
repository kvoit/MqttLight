#pragma once

#include<MqttBaseLight.hpp>
#include <Wire.h>
#include<Adafruit_PWMServoDriver.h>

class MqttPWMServoDriverLight : public MqttBaseLight
{
public:
    MqttPWMServoDriverLight(Adafruit_PWMServoDriver &pwm_a, uint8_t pwmnum_a, uint16_t pwm_offset_a, MqttController &mqtt, const char *mqtt_topic_control_state_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a);
    
    // Reference: the driver's I2C device only exists after begin() on the original object
    Adafruit_PWMServoDriver &pwm;
    const uint8_t pwmnum;
    const uint16_t pwm_offset;
    
    virtual void commit();
};