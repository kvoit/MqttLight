#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "NonlinearLight.h" //https://github.com/kvoit/NonlinearLight
#include <MqttController.hpp>

class MqttBaseLight : public MqttListener
{
public:
    MqttBaseLight(MqttController &mqtt, const char *mqtt_topic, uint8_t brightness, uint16_t min_pwm, uint16_t max_pwm)
        : MqttListener(mqtt,mqtt_topic), brightness(brightness), min_pwm(min_pwm), max_pwm(max_pwm)
    {};

    uint8_t getBrightness();

    void begin();
    void begin(const uint8_t brightness);

    bool presentMessage(const char *topic,const char *payload);

    void decreaseBrightness(uint8_t change = 4);
    void increaseBrightness(uint8_t change = 4);
    void changeBrightness(int8_t change);
    void setBrightness(int8_t bright_val);

    void switchOn();
    void switchOff();
    void toggleOnOff();

    void homeassistantDiscover(const char *name, const char *unique_id, const char *device_name=NULL, const char *device_unique_id=NULL);

protected:
    bool state = false;
    uint8_t brightness = 0;

    const uint16_t min_pwm;
    const uint16_t max_pwm;

    void (*reportCallback)(uint8_t);

    virtual void commit() = 0;
    void report();
    void setReportCallback(void (*reportCallback)(uint8_t));
};
