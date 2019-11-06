#include <INTERVAL.h> //https://forum.arduino.cc/index.php?topic=413734.0
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
#include <ESPTools.h> //https://github.com/kvoit/ESPTools
#include <MqttLight.h>
#include <NonlinearLight.h> //https://github.com/kvoit/NonlinearLight
#include <EEPROM.h>

#include "BaseConfig.h"

#define DEBUGLib_DEBUG
#include <DEBUGLib.h>

WiFiClient espClient;
PubSubClient mqttclient(espClient);
const unsigned long mqttReconnectInterval = 5000;

const char* mqtt_topic_tree                  = MQTT_TOPIC_BASE "light/#";
const char* mqtt_topic_pwm_freqency_cmd      = MQTT_TOPIC_BASE "light/pwm/frequency/cmd";
const char* mqtt_topic_pwm_freqency_state    = MQTT_TOPIC_BASE "light/pwm/frequency/state";

#define EEPROM_CONFIG_VERSION "ls2"
#define EEPROM_CONFIG_START 32

// Example settings structure
struct ConfigStruct {
  char version[4];
  uint16_t pwm_freq;
  uint8_t level;
} storage = {
  EEPROM_CONFIG_VERSION,
  200,
  50
};

MqttMOSFETLight light[] = {
  {D8, mqttclient, MQTT_TOPIC_BASE "light/direct/0", storage.level, 1, 1023}
};
uint8_t light_n = 1;

void loadConfig() {
  if (EEPROM.read(EEPROM_CONFIG_START + 0) == EEPROM_CONFIG_VERSION[0] &&
      EEPROM.read(EEPROM_CONFIG_START + 1) == EEPROM_CONFIG_VERSION[1] &&
      EEPROM.read(EEPROM_CONFIG_START + 2) == EEPROM_CONFIG_VERSION[2])
  {
    for (unsigned int t = 0; t < sizeof(storage); t++)
    {
      *((char*)&storage + t) = EEPROM.read(EEPROM_CONFIG_START + t);
    }
    DEBUG_PRINTLN("Reading EEPROM config successful");
  } else {
    DEBUG_PRINTLN("Reading EEPROM config failed");
  }
}

void saveConfig() {
  for (unsigned int t = 0; t < sizeof(storage); t++)
  {
    EEPROM.write(EEPROM_CONFIG_START + t, *((char*)&storage + t));
  }
  DEBUG_PRINTLN("Written EEPROM config");
  EEPROM.commit();
}

void saveToEEPROM(uint8_t level)
{
  DEBUG_PRINT("Saving to EEPROM: ");
  DEBUG_PRINTLN(level);

  if (storage.level != level)
  {
    storage.level = level;
    saveConfig();
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
  // Make const char* from byte*
  char message_buff[length + 1];
  int i;
  for (i = 0; i < length; i++)
  {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  const char *p_payload = message_buff;
  DEBUG_PRINT(topic);
  DEBUG_PRINT(": ");
  DEBUG_PRINTLN(p_payload);

  for (int i = 0; i < light_n; i++)
  {
    char topicstr[light[i].getMQTTCommandTopicLen() + 1];
    light[i].getMQTTCommandTopic(topicstr);
    DEBUG_PRINTLN(topicstr);
    if (!strcmp(topic, topicstr))
    {
      DEBUG_PRINT("Assigned to MQTTLight ");
      DEBUG_PRINTLN(i);
      light[i].parsePayload(p_payload);
      return;
    }
  }

  if (!strcmp(topic, mqtt_topic_pwm_freqency_cmd))
  {
    DEBUG_PRINT("Setting pwm frequency to");
    DEBUG_PRINTLN(atoi(p_payload));
    analogWriteFreq(atoi(p_payload));
    storage.pwm_freq = atoi(p_payload);
    saveConfig();
    char pubchar[5];
    sprintf(pubchar, "%d", atoi(p_payload));
    mqttclient.publish(mqtt_topic_pwm_freqency_state, pubchar, true);
    return;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  DEBUG_PRINTLN("DEBUG");
  
  EEPROM.begin(512);
  loadConfig();
  analogWriteFreq(storage.pwm_freq);
  light[0].setReportCallback(saveToEEPROM);
  light[0].begin(storage.level);

  Serial.println("Started ...");
  startWifi(ssid, password, device_name);
  Serial.println( "IP address: " );
  Serial.println( WiFi.localIP() );

  configArduinoOTA(device_name, ota_password);

  mqttclient.setServer(mqtt_server, 1883);
  mqttclient.setCallback(mqtt_callback);
  Serial.println("Initialization complete.");
}

void loop() {
  delay(2);
  if (!mqtt_check(mqttclient, mqttReconnectInterval, device_name, mqtt_user, mqtt_pw, mqtt_topic_tree)) {
    DEBUG_PRINT("MQTT reconnect failed, rc=");
    DEBUG_PRINT(mqttclient.state());
    DEBUG_PRINTLN(" try again in 5 seconds");
  }

  ArduinoOTA.handle();
}
