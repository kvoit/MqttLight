#include<MqttPWMServoDriverLight.hpp>

/* MqttPWMServoDriverLight */
MqttPWMServoDriverLight::MqttPWMServoDriverLight(Adafruit_PWMServoDriver &pwm_a, uint8_t pwmnum_a, uint16_t pwm_offset_a, MqttController &mqtt, const char *mqtt_topic_control_state_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a)
    : MqttBaseLight(mqtt, mqtt_topic_control_state_a, defaultLevel_a, min_pwm_a, max_pwm_a), pwm(pwm_a), pwmnum(pwmnum_a), pwm_offset(pwm_offset_a)
{ ;
}
    
void MqttPWMServoDriverLight::setLevel()
{
    pwm.setPWM(pwmnum, pwm_offset, (pwm_offset + NonlinearLight::toPWM(level, min_pwm, max_pwm))%4096);
    reportLevel();
}