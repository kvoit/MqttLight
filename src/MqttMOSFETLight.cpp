#include<MqttMOSFETLight.hpp>

/* MqttMOSFETLight */
MqttMOSFETLight::MqttMOSFETLight(uint8_t pin_a, MqttController &mqtt, const char *mqtt_topic_base_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a, int8_t channel, int32_t freq, int8_t res)
        : MqttBaseLight(mqtt, mqtt_topic_base_a, defaultLevel_a, min_pwm_a, max_pwm_a), pin(pin_a)
{ 
    #ifdef ESP32
    if(channel>-1) {
        ledcAttachPin(pin_a, channel);
        pin = channel;
    }
    if(freq>-1 && res>-1) {
        ledcSetup(pin, freq, res);
    }
    #else
    ;
    #endif
}

void MqttMOSFETLight::commit()
{
    #ifdef ESP32
    if(state) {
        ledcWrite(pin,NonlinearLight::toPWM(brightness, min_pwm, max_pwm));
    } else {
        ledcWrite(pin,NonlinearLight::toPWM(0, min_pwm, max_pwm));
    }
    #elif
    if(state) {
        analogWrite(pin,NonlinearLight::toPWM(brightness, min_pwm, max_pwm));
    } else {
        analogWrite(pin,NonlinearLight::toPWM(0, min_pwm, max_pwm));
    }
    #endif
    
    report();
}

void MqttMOSFETLight::begin(const uint8_t brightness)
{
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
    MqttBaseLight::begin(brightness);
}
