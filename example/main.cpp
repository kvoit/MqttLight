#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <MqttController.hpp>
#include <MqttMOSFETLight.hpp>

#include "BaseConfig.h"

WiFiClient espClient;

PubSubClient pubsubclient(mqtt_server, 1883, espClient);
MqttController mqtt_controller(pubsubclient,device_name,mqtt_user,mqtt_pw);
void mqtt_callback_func(const char* topic, const byte* payload, unsigned int length) { mqtt_controller.callback(topic, payload,length); }

MqttMOSFETLight mqttlight(D8, mqtt_controller, MQTT_TOPIC_BASE "/#",25,2,1023);

char pubchar[20];

void setup() {
  Serial.begin(115200);

  // Start Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(100);
  }

  // Configure pubsubclient
  pubsubclient.setServer(mqtt_server, 1883);
  pubsubclient.setCallback(mqtt_callback_func);

  // Initialize mqttlight (configures pin)
  mqttlight.begin(0);
}

void loop() {
  mqtt_controller.handle();
}