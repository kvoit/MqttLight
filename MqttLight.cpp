#include "DEBUGLib.h" //https://github.com/kvoit/DEBUGLib
#include "MqttLight.h"

/* MqttBaseLight */
MqttBaseLight::MqttBaseLight(PubSubClient &mqttclient_a, const char *mqtt_topic_base_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a) 
        : mqtt_topic_base(mqtt_topic_base_a), mqttclient(mqttclient_a), defaultLevel(defaultLevel_a), min_pwm(min_pwm_a), max_pwm(max_pwm_a), level(defaultLevel), lastLevel(defaultLevel)
{
}

void MqttBaseLight::begin()
{
    begin(defaultLevel);
}

void MqttBaseLight::begin(const uint8_t level)
{
    setBrightness(level);
    setLevel();
}

bool MqttBaseLight::parsePayload(const char *p_payload)
{
    if ( !strcmp(p_payload, "ON") )
    {
        setBrightness(lastLevel);
        setLevel();
        return true;
    }
    else if ( !strcmp(p_payload, "OFF") )
    {
        setBrightness(0);
        setLevel();
        return true;
    }
    else if ( !strcmp(p_payload, "DECREASE") )
    {
        decreaseBrightness();
        setLevel();
        return true;
    }
    else if ( !strcmp(p_payload, "INCREASE") )
    {
        increaseBrightness();
        setLevel();
        return true;
    }
    else
    {
        uint8_t bright_val = atoi(p_payload);
        char re_a[strlen(p_payload)];
        itoa(bright_val,re_a,10);
        if (!strcmp(re_a,p_payload))
        {
            setBrightness(bright_val);
            setLevel();
            return true;
        }
    }
    return false;
}

uint8_t MqttBaseLight::getLevel()
{
    return level;
}

void MqttBaseLight::decreaseBrightness(uint8_t change)
{
    DEBUG_PRINT("Decreasing brightness by ");
    DEBUG_PRINTLN(change);
    if (level > change) {
        level -= change;
    } else {
        level = 1;
    }
}

void MqttBaseLight::increaseBrightness(uint8_t change)
{
    DEBUG_PRINT("Increasing brightness by ");
    DEBUG_PRINTLN(change);
    if (100 - level > change) {
        level += change;
    } else {
        level = 100;
    }
}

void MqttBaseLight::decreaseBrightness()
{
    decreaseBrightness(4);
}

void MqttBaseLight::increaseBrightness()
{
    increaseBrightness(4);
}

void MqttBaseLight::changeBrightness(int8_t change)
{
    if (change > 0) {
        increaseBrightness(change);
    } else {
        decreaseBrightness(-1*change);
    }
}

void MqttBaseLight::setBrightness(int8_t bright_val)
{
    level = bright_val;
}

void MqttBaseLight::switchOn()
{
    level = lastLevel;
}

void MqttBaseLight::switchOff()
{
    if (level>0)
    {
        DEBUG_PRINT("Switching on to lastlevel ");
        DEBUG_PRINTLN(lastLevel);
        lastLevel = level;
        level = 0;
    }
}

void MqttBaseLight::toggleOnOff()
{
    if (level==0)
    {
        DEBUG_PRINTLN("Toggle: Switching on");
        switchOn();
    }
    else
    {
        DEBUG_PRINTLN("Toggle: Switching off");
        switchOff();
    }
}

void MqttBaseLight::getMQTTCommandTopic(char *topicstr)
{
    strcpy(topicstr, mqtt_topic_base); /* copy name into the new var */
    strcat(topicstr, "/cmd");
}

void MqttBaseLight::getMQTTStateTopic(char *topicstr)
{
    strcpy(topicstr, mqtt_topic_base); /* copy name into the new var */
    strcat(topicstr, "/state");
}

uint16_t MqttBaseLight::getMQTTStateTopicLen()
{
    return strlen(mqtt_topic_base)+6;
}

uint16_t MqttBaseLight::getMQTTCommandTopicLen()
{
    return strlen(mqtt_topic_base)+4;
}

void MqttBaseLight::reportLevel()
{
    if (setCallback) {
        setCallback(level);
    }
    
    char topicstr[getMQTTStateTopicLen()+1];
    getMQTTStateTopic(topicstr);
    
    char pubchar[5];
    sprintf(pubchar, "%d", (int)(level));
    if (mqttclient.publish(topicstr, pubchar, true))
    { ;
    } else
    { ;
    }
}

void MqttBaseLight::setReportCallback(void (*setCallback)(uint8_t)) {
    this->setCallback = setCallback;
}

// void MqttBaseLight::setLevel()
// { ;
// }

/* MqttMOSFETLight */
MqttMOSFETLight::MqttMOSFETLight(uint8_t pin_a, PubSubClient &mqttclient_a, const char *mqtt_topic_base_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a)
        : MqttBaseLight(mqttclient_a, mqtt_topic_base_a, defaultLevel_a, min_pwm_a, max_pwm_a), pin(pin_a)
{ ;
}

void MqttMOSFETLight::setLevel()
{
    DEBUG_PRINT("Setting pin ");
    DEBUG_PRINT(pin);
    DEBUG_PRINT(" to ");
    DEBUG_PRINTLN(level);
    DEBUG_PRINTLN(NonlinearLight::toPWM(level, min_pwm, max_pwm));
    analogWrite(pin,NonlinearLight::toPWM(level, min_pwm, max_pwm));
    reportLevel();
}

void MqttMOSFETLight::begin(const uint8_t level)
{
    digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
    MqttBaseLight::begin(level);
}

/* MqttPWMServoDriverLight */
MqttPWMServoDriverLight::MqttPWMServoDriverLight(Adafruit_PWMServoDriver &pwm_a, uint8_t pwmnum_a, uint16_t pwm_offset_a, PubSubClient &mqttclient_a, const char *mqtt_topic_control_state_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a)
    : MqttBaseLight(mqttclient_a, mqtt_topic_control_state_a, defaultLevel_a, min_pwm_a, max_pwm_a), pwm(pwm_a), pwmnum(pwmnum_a), pwm_offset(pwm_offset_a)
{ ;
}
    
void MqttPWMServoDriverLight::setLevel()
{
    pwm.setPWM(pwmnum, pwm_offset, (pwm_offset + NonlinearLight::toPWM(level, min_pwm, max_pwm))%4096);
    reportLevel();
}
