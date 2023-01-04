#include<MqttBaseLight.hpp>

void MqttBaseLight::begin()
{
    begin(brightness);
}

void MqttBaseLight::begin(const uint8_t brightness)
{
    setBrightness(brightness);
    commit();
}

bool MqttBaseLight::presentMessage(const char *topic,const char *payload)
{
    if(!strncmp(topic,this->getMQTTTopic(),baselength)) {
        if(!strcmp(&topic[baselength],"/brightness/set")) {
            if ( !strcmp(payload, "DECREASE") )
            {
                decreaseBrightness();
                commit();
                return true;
            }
            else if ( !strcmp(payload, "INCREASE") )
            {
                increaseBrightness();
                commit();
                return true;
            }
            else
            {
                uint8_t bright_val = atof(payload);
                setBrightness(bright_val);
                commit();
                return true;
            }
        }
        else if(!strcmp(&topic[baselength],"/switch/set")) {
            if ( !strcmp(payload, "ON") || !strcmp(payload, "on") )
            {
                switchOn();
                commit();
                return true;
            }
            else if ( !strcmp(payload, "OFF") || !strcmp(payload, "off") )
            {
                switchOff();
                commit();
                return true;
            }
        } else {

        }
    }
    return false;
}


uint8_t MqttBaseLight::getBrightness()
{
    return brightness;
}

void MqttBaseLight::decreaseBrightness(uint8_t changebrightness)
{
    if (brightness > changebrightness) {
        setBrightness(brightness-changebrightness);
    } else {
        setBrightness(1);
    }
}

void MqttBaseLight::increaseBrightness(uint8_t changebrightness)
{
    if (100 - brightness > changebrightness) {
        setBrightness(brightness+changebrightness);
    } else {
        setBrightness(100);
    }
}

void MqttBaseLight::changeBrightness(int8_t changebrightness)
{
    if (changebrightness > 0) {
        increaseBrightness(changebrightness);
    } else {
        decreaseBrightness(-1*changebrightness);
    }
}

void MqttBaseLight::setBrightness(int8_t setbrightness)
{
    brightness = setbrightness;
    if(brightness>0) {
        state = true;
    } else {
        state = false;
    }

}

void MqttBaseLight::switchOn()
{
    state = true;
}

void MqttBaseLight::switchOff()
{
    state = false;
}

void MqttBaseLight::toggleOnOff()
{
    state = !state;
}

void MqttBaseLight::report()
{
    uint8_t newbrightness = brightness;
    if(!state)
        newbrightness = 0;

    if (reportCallback) {
        reportCallback(newbrightness);
    }

    const uint8_t topiclength = baselength+1+12;
    char statetopic[topiclength];
    strncpy(statetopic,this->getMQTTTopic(),baselength);

    char pubchar[5];

    strncpy(&statetopic[baselength],"/switch",topiclength-baselength-1);
    strncpy(pubchar,state?"ON":"OFF",5);
    if (mqtt_controller.sendMessage(statetopic, pubchar, true))
    { ;
    } else
    { ;
    }
    mqtt_controller.handle();

    strncpy(&statetopic[baselength],"/brightness",topiclength-baselength-1);
    sprintf(pubchar, "%d", newbrightness);
    if (mqtt_controller.sendMessage(statetopic, pubchar, true))
    { ;
    } else
    { ;
    }
    mqtt_controller.handle();
}

void MqttBaseLight::setReportCallback(void (*reportCallback)(uint8_t)) {
    this->reportCallback = reportCallback;
}

void MqttBaseLight::homeassistantDiscover(const char *name, const char *unique_id, const char *device_name, const char *device_unique_id)
{
    if(device_name==NULL) {
        device_name = name;
    }
    if(device_unique_id==NULL) {
        device_unique_id = unique_id;
    }
    DynamicJsonDocument doc(1024);

    char discovery_msg[1024]{};

    const uint8_t topiclength = baselength+20;
    char command_topic[topiclength]{};
    strncpy(command_topic,this->getMQTTTopic(),baselength);
    strncpy(&command_topic[baselength],"/switch/set",topiclength-baselength-1);
    char state_topic[topiclength]{};
    strncpy(state_topic,this->getMQTTTopic(),baselength);
    strncpy(&state_topic[baselength],"/switch",topiclength-baselength-1);
    char brightness_state_topic[topiclength]{};
    strncpy(brightness_state_topic,this->getMQTTTopic(),baselength);
    strncpy(&brightness_state_topic[baselength],"/brightness",topiclength-baselength-1);
    char brightness_command_topic[topiclength]{};
    strncpy(brightness_command_topic,this->getMQTTTopic(),baselength);
    strncpy(&brightness_command_topic[baselength],"/brightness/set",topiclength-baselength-1);

    char discovery_topic[128]{};

    mqtt_controller.handle();
    delay(100);
    mqtt_controller.handle();
    delay(100);

    snprintf(discovery_topic, sizeof(discovery_topic), "homeassistant/light/mqttlight/light_%s/config", name);

    doc["name"] = name;
    doc["unique_id"] = unique_id;
    doc["state_topic"] = state_topic;
    doc["command_topic"] = command_topic;
    doc["brightness_state_topic"] = brightness_state_topic;
    doc["brightness_command_topic"] = brightness_command_topic;
    doc["brightness_scale"] = 100;
    doc["optimistic"] = false;
    doc["retain"] = true;
    doc["device"]["name"] = device_name;
    doc["device"]["identifiers"][0] = device_unique_id;

    serializeJson(doc, discovery_msg);
    mqtt_controller.handle();
    mqtt_controller.sendMessage(discovery_topic, discovery_msg, true);
    delay(50);
    mqtt_controller.handle();
}
