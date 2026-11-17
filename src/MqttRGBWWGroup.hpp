#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <MqttController.hpp>
#include <MqttBaseLight.hpp>
#include <vector>

class MqttRGBWWGroup : public MqttBaseLight
{
public:
    MqttRGBWWGroup(MqttLightPtr *mqttLights, uint8_t n_mqttLights, MqttController &mqtt, const char *mqtt_topic, uint8_t brightness)
        : MqttBaseLight(mqtt,mqtt_topic, brightness, 0, 0), mqttLights(mqttLights), n_mqttLights(n_mqttLights)
    {};

    virtual void begin();
    virtual void begin(const uint8_t brightness);

    virtual bool presentMessage(const char *topic,const char *payload);

    // virtual void decreaseBrightness(uint8_t change = 4);
    // virtual void increaseBrightness(uint8_t change = 4);
    // virtual void changeBrightness(int8_t change);
    // virtual void setBrightness(int8_t bright_val);

    // virtual void switchOn();
    // virtual void switchOff();
    // virtual void toggleOnOff();

    virtual void homeassistantDiscover(const char *name, const char *unique_id, const char *device_name=NULL, const char *device_unique_id=NULL);

protected:
    MqttLightPtr *mqttLights; 
    const uint8_t n_mqttLights;

    uint8_t color_values[5] = {255,255,255,255,255}; 

    virtual void commit();
    virtual void report();
};
