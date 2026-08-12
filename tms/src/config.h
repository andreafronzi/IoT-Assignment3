#ifndef __CONFIG__
#define __CONFIG__

#include <WiFi.h>
#include <PubSubClient.h>

#define MSG_BUFFER_SIZE 50

const char *ssid = "LittleBarfly";
const char *password = "esiot2025";

/* MQTT server address */
const char *mqtt_server = "broker.mqtt-dashboard.com";

/* MQTT topic */
const char *topic = "waterData";

#endif