#ifndef MqttLight_h
#define MqttLight_h
#include "Arduino.h"
#include "NonlinearLight.h" //https://github.com/kvoit/NonlinearLight
#include "PubSubClient.h" //https://github.com/knolleary/pubsubclient

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

class MqttBaseLight
{
public:
    MqttBaseLight(PubSubClient &mqttclient_a, const char *mqtt_topic_base_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a);
    
    const char *mqtt_topic_base;
    const uint8_t defaultLevel;
    const uint16_t min_pwm;
    const uint16_t max_pwm;
    void (*setCallback)(uint8_t);
    
    PubSubClient mqttclient;
    uint8_t level;
    uint8_t lastLevel;
    
    virtual void setLevel() = 0;
    void begin();
    void begin(const uint8_t level);
    bool parsePayload(const char *payload);
    uint8_t getLevel();
    void decreaseBrightness(uint8_t change);
    void increaseBrightness(uint8_t change);
    void decreaseBrightness();
    void increaseBrightness();
    void changeBrightness(int8_t change);
    void setBrightness(int8_t bright_val);
    void switchOn();
    void switchOff();
    void toggleOnOff();
    void getMQTTStateTopic(char *topicstr);
    void getMQTTCommandTopic(char *topicstr);
    uint16_t getMQTTStateTopicLen();
    uint16_t getMQTTCommandTopicLen();
    void reportLevel();
    void setReportCallback(void (*setCallback)(uint8_t));
};

class MqttMOSFETLight : public MqttBaseLight
{
public:
    MqttMOSFETLight(uint8_t pin_a, PubSubClient &mqttclient_a,const char *mqtt_topic_control_state_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a);
    
    const uint8_t pin;
    
    void setLevel();
    void begin(const uint8_t level);
};

class MqttPWMServoDriverLight : public MqttBaseLight
{
public:
    MqttPWMServoDriverLight(Adafruit_PWMServoDriver &pwm_a, uint8_t pwmnum_a, uint16_t pwm_offset_a, PubSubClient &mqttclient_a, const char *mqtt_topic_control_state_a, uint8_t defaultLevel_a, uint16_t min_pwm_a, uint16_t max_pwm_a);
    
    Adafruit_PWMServoDriver pwm;
    const uint8_t pwmnum;
    const uint16_t pwm_offset;
    
    void setLevel();
    void begin(const uint8_t level);
};

#endif
