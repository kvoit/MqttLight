#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include "NonlinearLight.h" //https://github.com/kvoit/NonlinearLight
#include <MqttController.hpp>

class MqttBaseLight : public MqttListener
{
public:
    MqttBaseLight(MqttController &mqtt, const char *mqtt_topic, uint8_t defaultLevel, uint16_t min_pwm, uint16_t max_pwm)
        : MqttListener(mqtt,mqtt_topic), mqtt(mqtt), mqtt_topic(mqtt_topic), defaultLevel(defaultLevel), level(defaultLevel), lastLevel(defaultLevel), min_pwm(min_pwm), max_pwm(max_pwm) 
    {
        strcat(mqtt_state_topic, mqtt_topic);
        strcat(mqtt_state_topic, "/state");
    }; 
    
    MqttController &mqtt;

    const char* mqtt_topic;
    char mqtt_state_topic[64] = {};

    bool state = false;
    
    const uint8_t defaultLevel;
    uint8_t level;
    uint8_t lastLevel;

    const uint16_t min_pwm;
    const uint16_t max_pwm;

    void (*setCallback)(uint8_t);
    
    
    virtual void setLevel() = 0;
    void begin();
    void begin(const uint8_t level);
    bool parsePayload(const char *payload);
    bool presentMessage(const char *topic,const char *payload);
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
    const char* getMQTTStateTopic();
    const char* getMQTTTopic();
    void reportLevel(uint8_t);
    void setReportCallback(void (*setCallback)(uint8_t));
};
