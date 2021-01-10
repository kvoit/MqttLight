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
        setBrightness(brightness-changebrightness);
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

    uint8_t topiclength = baselength+1+12;
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