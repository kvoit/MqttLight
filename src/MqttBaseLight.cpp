#include<MqttBaseLight.hpp>

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
        // // Discard 
        // for (uint8_t i = 0; i<strlen(p_payload);i++) {
        //     if(p_payload[i] == '.')
        //         p_payload[i] = 0;
        // }
        uint8_t bright_val = atof(p_payload);
        setBrightness(bright_val);
        setLevel();
        return true;
    }
    return false;
}

uint8_t MqttBaseLight::getLevel()
{
    return level;
}

void MqttBaseLight::decreaseBrightness(uint8_t change)
{
    
    
    if (level > change) {
        level -= change;
    } else {
        level = 1;
    }
}

void MqttBaseLight::increaseBrightness(uint8_t change)
{
    
    
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
        
        
        lastLevel = level;
        level = 0;
    }
}

void MqttBaseLight::toggleOnOff()
{
    if (level==0)
    {
        
        switchOn();
    }
    else
    {
        
        switchOff();
    }
}

const char* MqttBaseLight::getMQTTTopic()
{
    return mqtt_topic;
}

const char* MqttBaseLight::getMQTTStateTopic()
{
    return mqtt_state_topic;
}

void MqttBaseLight::reportLevel()
{
    if (setCallback) {
        setCallback(level);
    }

    char pubchar[5];
    sprintf(pubchar, "%d", (int)(level));
    if (mqtt.sendMessage(getMQTTStateTopic(), pubchar, true))
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
