#ifndef __CONFIG__
#define __CONFIG__

#include <WiFi.h>
#include <PubSubClient.h>

#define ECHO_PIN 18
#define TRIG_PIN 5
#define MAX_TIME 100

#define LED_RED_PIN 21
#define LED_GREEN_PIN 19

#define MSG_BUFFER_SIZE 50
#define TANK_HEIGHT 100.0

const char *ssid = "Wind3 HUB-D65C41";
const char *password = "1s5gtr2si7f2adzp";

/* MQTT server address */
const char *mqtt_server = "mqtt-dashboard.com";

/* MQTT topic */
const char *topic = "waterData";

#endif