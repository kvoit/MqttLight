#include<MqttMOSFETLight.hpp>

/* MqttMOSFETLight */
MqttMOSFETLight::MqttMOSFETLight(uint8_t pin_a, MqttController &mqtt, const char *mqtt_topic_base_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a, int8_t channel, int32_t freq, int8_t res)
        : MqttBaseLight(mqtt, mqtt_topic_base_a, defaultLevel_a, min_pwm_a, max_pwm_a), pin(pin_a), channel(channel), freq(freq), res(res)
{ 
}

void MqttMOSFETLight::commit()
{
    int value;
    if(state) {
        value = NonlinearLight::toPWM(brightness, min_pwm, max_pwm);
    } else {
        value = NonlinearLight::toPWM(0, min_pwm, max_pwm);
    }
    
    #ifdef ESP32
    ledcWrite(pin,value);
    #else
    analogWrite(pin,value);
    #endif

    report();
}

void MqttMOSFETLight::begin()
{
    begin(brightness);
}

void MqttMOSFETLight::begin(const uint8_t brightness)
{
    // Output low until the first commit()
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);

    #ifdef ESP32
    if(channel>-1) {
        if(freq>-1 && res>-1) {
            ledcSetup(channel, freq, res);
        }
        ledcAttachPin(pin, channel);
        // ledcWrite() takes the channel, so pin holds the channel from here on
        pin = channel;
    }
    #endif

    MqttBaseLight::begin(brightness);
}
