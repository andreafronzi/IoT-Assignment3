#include <Arduino.h>
#include "config.h"
#include "devices/sonar/Sonar.h"
#include "devices/sonar/ProximitySensor.h"
#include "devices/led/Led.h"
#include "devices/led/Light.h"

typedef enum
{
  NETWORK_OFFLINE,
  NETWORK_ONLINE,
} communicationState;

communicationState state;

TaskHandle_t communicationTaskHandle;
TaskHandle_t sensorTaskHandle;

WiFiClient espClient;
PubSubClient client(espClient);

boolean justEntered;

Sonar sonar;

Led ledRed;
Led ledGreen;

unsigned long lastMsgTime = 0;
char msg[MSG_BUFFER_SIZE];
int waterLevel = 0;

void setup()
{
  randomSeed(micros());
  setupWifi();
  setupMqtt();
  Serial.begin(115200);
  xTaskCreate(communicationTask, "Communication Task", 256, NULL, 1, &communicationTaskHandle);
  xTaskCreate(sensorTask, "Sensor Task", 256, NULL, 1, &sensorTaskHandle);
}

void setupWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMqtt()
{
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println(String("Message arrived on [") + topic + "] len: " + length + " txt: " + String((char *)payload, length));
}


void reconnect()
{
  while (!client.connected())
  {
    // Create a random client ID
    String clientId = String("waterLevelClient-") + String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      client.subscribe(topic);
    }
    else
    {
      delay(5000);
    }
  }
}

void communicationTask(void *parameter)
{
  while (true)
  {
    switch (state)
    {
    case NETWORK_OFFLINE:
      reconnect();
      if (client.connected())
      {
        state = NETWORK_ONLINE;
        justEntered = true;
      }
      break;
    case NETWORK_ONLINE:
      sendMessagge();
      if (!client.connected())
      {
        state = NETWORK_OFFLINE;
        justEntered = true;
      }
      break;
    }
    // aggiungere delay per una frequenza voluta di invio dei messaggi
  }
}

void sensorTask(void *parameter)
{
  while (true)
  {
    switch (state)
    {
    case NETWORK_OFFLINE:
      if (justEntered)
      {
        ledRed.switchOn();
        ledGreen.switchOff();
        justEntered = false;
      }
      waterLevel = sonar.getDistance();
      break;
    case NETWORK_ONLINE:
      if (justEntered)
      {
        ledRed.switchOff();
        ledGreen.switchOn();
        justEntered = false;
      }
      break;
    }
  }
  // aggiungere delay per una frequenza voluta di lettura del sensore
}

void sendMessagge()
{
  snprintf(msg, sizeof(msg), "%.2f", waterLevel);
  if (client.connected())
  {
    client.publish(topic, msg);
  }
}

void loop()
{
}
