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

volatile communicationState state;

TaskHandle_t communicationTaskHandle;
TaskHandle_t sensorTaskHandle;

WiFiClient espClient;
PubSubClient client(espClient);

volatile boolean justEntered;

ProximitySensor *sonar;

Led *ledRed;
Led *ledGreen;

unsigned long lastMsgTime = 0;
char msg[MSG_BUFFER_SIZE];
volatile double waterLevel = 0.0;

void sendMessagge()
{
  snprintf(msg, sizeof(msg), "%.2f", waterLevel);
  if (client.connected())
  {
    client.publish(topic, msg);
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println(String("Message arrived on [") + topic + "] len: " + length + " txt: " + String((char *)payload, length));
}

void setupWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  vTaskDelay(pdMS_TO_TICKS(5000));
  if (WiFi.status() == WL_CONNECTED)
  {
    state = NETWORK_ONLINE;
    justEntered = true;
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    state = NETWORK_OFFLINE;
    justEntered = true;
    Serial.println("");
    Serial.println("WiFi connection failed on boot, will retry in background.");
  }
}

void setupMqtt()
{
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void reconnect()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      vTaskDelay(pdMS_TO_TICKS(5000));
      Serial.print(".");
    }
    Serial.println("\nWiFi reconnected.");
  }

  while (!client.connected())
  {
    // Create a random client ID
    String clientId = String("clientId-") + String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      client.subscribe(topic);
    }
    else
    {
      vTaskDelay(pdMS_TO_TICKS(5000));
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
      if (client.connected() && WiFi.status() == WL_CONNECTED)
      {
        state = NETWORK_ONLINE;
        justEntered = true;
      }
      break;
    case NETWORK_ONLINE:
      client.loop();
      if (millis() - lastMsgTime > 1000)
      {
        sendMessagge();
        lastMsgTime = millis();
      }
      if (!client.connected() || WiFi.status() != WL_CONNECTED)
      {
        state = NETWORK_OFFLINE;
        justEntered = true;
      }
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(300));
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
        ledRed->switchOn();
        ledGreen->switchOff();
        justEntered = false;
      }
      break;
    case NETWORK_ONLINE:
      if (justEntered)
      {
        ledRed->switchOff();
        ledGreen->switchOn();
        justEntered = false;
      }
      if(double tmp = sonar->getDistance() > TANK_HEIGHT){
        waterLevel = 0.0;
      } else {
        waterLevel = TANK_HEIGHT - tmp;
      }
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(300));
  }
}

void setup()
{
  Serial.begin(115200);
  ledRed = new Led(LED_RED_PIN);
  ledGreen = new Led(LED_GREEN_PIN);
  sonar = new Sonar(ECHO_PIN, TRIG_PIN, MAX_TIME);
  randomSeed(micros());
  setupWifi();
  setupMqtt();
  xTaskCreate(communicationTask, "CommunicationTask", 4096, NULL, 1, &communicationTaskHandle);
  xTaskCreate(sensorTask, "SensorTask", 2048, NULL, 1, &sensorTaskHandle);
}

void loop()
{
}
