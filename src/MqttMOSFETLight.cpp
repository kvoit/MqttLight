#include<MqttMOSFETLight.hpp>

/* MqttMOSFETLight */
MqttMOSFETLight::MqttMOSFETLight(uint8_t pin_a, MqttController &mqtt, const char *mqtt_topic_base_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a)
        : MqttBaseLight(mqtt, mqtt_topic_base_a, defaultLevel_a, min_pwm_a, max_pwm_a), pin(pin_a)
{ ;
}

void MqttMOSFETLight::commit()
{
    if(state) {
        analogWrite(pin,NonlinearLight::toPWM(brightness, min_pwm, max_pwm));
    } else {
        analogWrite(pin,NonlinearLight::toPWM(0, min_pwm, max_pwm));
    }
    
    report();
}

void MqttMOSFETLight::begin(const uint8_t brightness)
{
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
    MqttBaseLight::begin(brightness);
}
