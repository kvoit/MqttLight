#include<MqttMOSFETLight.hpp>

/* MqttMOSFETLight */
MqttMOSFETLight::MqttMOSFETLight(uint8_t pin_a, MqttController &mqtt, const char *mqtt_topic_base_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a)
        : MqttBaseLight(mqtt, mqtt_topic_base_a, defaultLevel_a, min_pwm_a, max_pwm_a), pin(pin_a)
{ ;
}

void MqttMOSFETLight::setLevel()
{
    uint8_t newlevel;
    if(state) {
        newlevel = level;
    } else {
        newlevel = 0;
    }
    analogWrite(pin,NonlinearLight::toPWM(newlevel, min_pwm, max_pwm));
    reportLevel(newlevel);
}

void MqttMOSFETLight::begin(const uint8_t level)
{
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
    MqttBaseLight::begin(level);
}
