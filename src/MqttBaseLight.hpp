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
    bool switchedOn() { return state; }

    virtual void begin();
    virtual void begin(const uint8_t brightness);

    virtual bool presentMessage(const char *topic,const char *payload);

    // Brightness and on/off state are independent: setting the brightness does not
    // switch the light on or off, so a brightness can be preset while it is off
    virtual void decreaseBrightness(uint8_t change = 4);
    virtual void increaseBrightness(uint8_t change = 4);
    virtual void changeBrightness(int8_t change);
    virtual void setBrightness(int8_t bright_val);

    virtual void switchOn();
    virtual void switchOff();
    virtual void toggleOnOff();

    virtual void homeassistantDiscover(const char *name, const char *unique_id, const char *device_name=NULL, const char *device_unique_id=NULL);
    // Applies brightness and state to the output and reports them
    virtual void commit() = 0;

protected:
    bool state = false;
    uint8_t brightness = 0;

    const uint16_t min_pwm;
    const uint16_t max_pwm;

    void (*reportCallback)(uint8_t) = nullptr;

    virtual void report();
    void setReportCallback(void (*reportCallback)(uint8_t));
};

typedef MqttBaseLight* MqttLightPtr;
