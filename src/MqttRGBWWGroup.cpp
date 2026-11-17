#include<MqttRGBWWGroup.hpp>

void MqttRGBWWGroup::begin()
{
    for (uint8_t i = 0; i<n_mqttLights;i++) {
        mqttLights[i]->begin();
    }
}

void MqttRGBWWGroup::begin(const uint8_t brightness)
{
    for (uint8_t i = 0; i<n_mqttLights;i++) {
        mqttLights[i]->begin(brightness);
    }
}

bool MqttRGBWWGroup::presentMessage(const char *topic,const char *payload)
{
    // #ifdef MQTTLIGHTDEBUG
    // Serial.printf("Received (%s) in (%s), checking (%s)\n", payload, topic, &topic[baselength]);
    // #endif
    if(!strncmp(topic,this->getMQTTTopic(),baselength)) {
        if(
            !strcmp(&topic[baselength],"/color_temp/set") ||
            !strcmp(&topic[baselength],"/rgb/set") ||
            !strcmp(&topic[baselength],"/rgbw/set") ||
            !strcmp(&topic[baselength],"/rgbww/set") ||
            !strcmp(&topic[baselength],"/group/set") 
            ) {
            if ( !strcmp(payload, "DECREASE") )
            {
                decreaseBrightness();
            }
            else if ( !strcmp(payload, "INCREASE") )
            {
                increaseBrightness();
            }
            else
            {
                char *pch = strtok((char*)payload, ",");

                for(uint8_t i = 0; i < n_mqttLights; i++ ) {
                    if (pch == NULL) {
                        break;
                    }
                    color_values[i] = atoi(pch);
                    pch = strtok(NULL, ",");
                }
            }
        }
        else if(!strcmp(&topic[baselength],"/brightness/set")) {
            // Brightness in percent; ignore non-numbers, clamp to 0-100
            char *end = nullptr;
            float bright_val = strtof(payload, &end);
            if (end == payload) {
                return true;
            }
            if (bright_val < 0) {
                bright_val = 0;
            } else if (bright_val > 100) {
                bright_val = 100;
            }
            setBrightness((uint8_t)bright_val);
        }
        else if(!strcmp(&topic[baselength],"/switch/set")) {
            // #ifdef MQTTLIGHTDEBU
            // Serial.printf("Received %s in %s\n", payload, "/switch/set");
            // #endif
            if ( !strcmp(payload, "ON") || !strcmp(payload, "on") )
            {
                switchOn();
            }
            else if ( !strcmp(payload, "OFF") || !strcmp(payload, "off") )
            {
                switchOff();
            }
        } else {
            return false;
        }
        commit();
        return true;
    }
    return false;
}

// void MqttRGBWWGroup::decreaseBrightness(uint8_t changebrightness)
// {
//     for (uint8_t i = 0; i<n_mqttLights;i++) {
//         mqttLights[i]->decreaseBrightness(changebrightness);
//     }
// }

// void MqttRGBWWGroup::increaseBrightness(uint8_t changebrightness)
// {
//     for (uint8_t i = 0; i<n_mqttLights;i++) {
//         mqttLights[i]->increaseBrightness(changebrightness);
//     }
// }

// void MqttRGBWWGroup::changeBrightness(int8_t changebrightness)
// {
//     for (uint8_t i = 0; i<n_mqttLights;i++) {
//         mqttLights[i]->changeBrightness(changebrightness);
//     }
// }

// void MqttRGBWWGroup::setBrightness(int8_t setbrightness)
// {
//     for (uint8_t i = 0; i<n_mqttLights;i++) {
//         mqttLights[i]->setBrightness(setbrightness);
//     }
// }

// void MqttRGBWWGroup::switchOn()
// {
//     for (uint8_t i = 0; i<n_mqttLights;i++) {
//         mqttLights[i]->switchOn();
//     }
// }

// void MqttRGBWWGroup::switchOff()
// {
//     for (uint8_t i = 0; i<n_mqttLights;i++) {
//         mqttLights[i]->switchOff();
//     }
// }

// void MqttRGBWWGroup::toggleOnOff()
// {
//     for (uint8_t i = 0; i<n_mqttLights;i++) {
//         mqttLights[i]->toggleOnOff();
//     }
// }

void MqttRGBWWGroup::commit()
{
    for(uint8_t i = 0; i < n_mqttLights; i++ ) {
        #ifdef MQTTLIGHTDEBUG
        Serial.printf("Sending to light %d (%d) from (%d), (%d)\n", i, (int)((float)color_values[i]*(float)brightness/255.0f), color_values[i], brightness);
        #endif
        mqttLights[i]->setBrightness((int)((float)color_values[i]*(float)brightness/255.0f));
        if(state){
            mqttLights[i]->switchOn();
        } else {
            mqttLights[i]->switchOff();
        }
    }
    for(uint8_t i = 0; i < n_mqttLights; i++ ) {
        #ifdef MQTTLIGHTDEBUG
        Serial.printf("Committing light %d\n", i);
        #endif
        mqttLights[i]->commit();
    }

    report();
}

void MqttRGBWWGroup::report()
{
    MqttBaseLight::report();

    char topic[baselength+12+1];
    strncpy(topic,this->getMQTTTopic(),baselength);
    
    
    if(n_mqttLights == 2) {
        strncpy(&topic[baselength],"/color_temp",sizeof(topic)-baselength-1);
    } else if(n_mqttLights == 3) {
        strncpy(&topic[baselength],"/rgb",sizeof(topic)-baselength-1);
    } else if(n_mqttLights == 4) {
        strncpy(&topic[baselength],"/rgbw",sizeof(topic)-baselength-1);
    } else if(n_mqttLights == 5) {
        strncpy(&topic[baselength],"/rgbww",sizeof(topic)-baselength-1);
    } else {
        strncpy(&topic[baselength],"/group",sizeof(topic)-baselength-1);
    }

    char message[n_mqttLights*4]{};
    char buffer[4];
    for(uint8_t i = 0; i < n_mqttLights; i++ ) {
        sprintf(buffer, "%d", color_values[i]);    // Convert int to string
        strncat(message, buffer, sizeof(message));               // Append number to result

        if (i < n_mqttLights - 1) {                      // Add comma except after last number
            strncat(message, ",", sizeof(message));
        }
    }

    if (mqtt_controller.sendMessage(topic, message, true))
    { ;
    } else
    { ;
    }
}

void MqttRGBWWGroup::homeassistantDiscover(const char *name, const char *unique_id, const char *device_name, const char *device_unique_id)
{
    if(device_name==NULL) {
        device_name = name;
    }
    if(device_unique_id==NULL) {
        device_unique_id = unique_id;
    }
    char discovery_msg[1024]{};
    JsonDocument doc;

    doc["name"] = name;
    doc["unique_id"] = unique_id;   

    const uint8_t topiclength = baselength+20;

    char command_topic[topiclength]{};
    strncpy(command_topic,this->getMQTTTopic(),baselength);
    strncpy(&command_topic[baselength],"/switch/set",topiclength-baselength-1);
    doc["command_topic"] = command_topic;
    
    char state_topic[topiclength]{};
    strncpy(state_topic,this->getMQTTTopic(),baselength);
    strncpy(&state_topic[baselength],"/switch",topiclength-baselength-1);
    doc["state_topic"] = state_topic;

    char brightness_state_topic[topiclength]{};
    strncpy(brightness_state_topic,this->getMQTTTopic(),baselength);
    strncpy(&brightness_state_topic[baselength],"/brightness",topiclength-baselength-1);
    doc["brightness_state_topic"] = brightness_state_topic;

    char brightness_command_topic[topiclength]{};
    strncpy(brightness_command_topic,this->getMQTTTopic(),baselength);
    strncpy(&brightness_command_topic[baselength],"/brightness/set",topiclength-baselength-1);
    doc["brightness_command_topic"] = brightness_command_topic;

    char color_command_topic[topiclength]{};
    const size_t N_mqttLights =  n_mqttLights;
    if(N_mqttLights == 2) {
        strncpy(color_command_topic,this->getMQTTTopic(),baselength);
        strncpy(&color_command_topic[baselength],"/color_temp/set",sizeof(color_command_topic)-baselength-1);
        doc["color_temp_command_topic"] = color_command_topic;
    } else if(N_mqttLights == 3) {
        strncpy(color_command_topic,this->getMQTTTopic(),baselength);
        strncpy(&color_command_topic[baselength],"/rgb/set",sizeof(color_command_topic)-baselength-1);
        doc["rgb_command_topic"] = color_command_topic;
    } else if(N_mqttLights == 4) {
        strncpy(color_command_topic,this->getMQTTTopic(),baselength);
        strncpy(&color_command_topic[baselength],"/rgbw/set",sizeof(color_command_topic)-baselength-1);
        doc["rgbw_command_topic"] = color_command_topic;
    } else if(N_mqttLights == 5) {
        strncpy(color_command_topic,this->getMQTTTopic(),baselength);
        strncpy(&color_command_topic[baselength],"/rgbww/set",sizeof(color_command_topic)-baselength-1);
        doc["rgbww_command_topic"] = color_command_topic;
    }

    char color_state_topic[topiclength]{};
    if(N_mqttLights == 2) {
        strncpy(color_state_topic,this->getMQTTTopic(),baselength);
        strncpy(&color_state_topic[baselength],"/color_temp",sizeof(color_state_topic)-baselength-1);
        doc["color_temp_state_topic"] = color_state_topic;
    } else if(N_mqttLights == 3) {
        strncpy(color_state_topic,this->getMQTTTopic(),baselength);
        strncpy(&color_state_topic[baselength],"/rgb",sizeof(color_state_topic)-baselength-1);
        doc["rgb_state_topic"] = color_state_topic;
    } else if(N_mqttLights == 4) {
        strncpy(color_state_topic,this->getMQTTTopic(),baselength);
        strncpy(&color_state_topic[baselength],"/rgbw",sizeof(color_state_topic)-baselength-1);
        doc["rgbw_state_topic"] = color_state_topic;
    } else if(N_mqttLights == 5) {
        strncpy(color_state_topic,this->getMQTTTopic(),baselength);
        strncpy(&color_state_topic[baselength],"/rgbww",sizeof(color_state_topic)-baselength-1);
        doc["rgbww_state_topic"] = color_state_topic;
    }

    doc["brightness_scale"] = 100;
    doc["optimistic"] = false;
    doc["retain"] = false;
    doc["device"]["name"] = device_name;
    doc["device"]["identifiers"][0] = device_unique_id;

    serializeJson(doc, discovery_msg);

    char discovery_topic[128]{};
    snprintf(discovery_topic, sizeof(discovery_topic), "homeassistant/light/mqttlight/light_%s/config", name);


    mqtt_controller.sendMessage(discovery_topic, discovery_msg, true);
}
