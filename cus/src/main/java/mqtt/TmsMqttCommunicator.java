package mqtt;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

/**
 * Gestisce la comunicazione MQTT con il Tank Monitoring Subsystem (ESP32).
 * Implementa il pattern Watchdog per monitorare lo stato della connessione
 * (Requisito T2).
 */
public class TmsMqttCommunicator {

    private MqttClient client;
    private final String brokerUrl;
    private final String topic;
    private final EspData espData;

    public TmsMqttCommunicator(final String brokerUrl, final String topic, final EspData espData) {
        this.brokerUrl = brokerUrl;
        this.topic = topic;
        this.espData = espData;
    }

    public void start() {
        try {
            // Un ClientID univoco previene conflitti se si riavvia il server velocemente
            String clientId = "CUS_Backend_" + System.currentTimeMillis();
            client = new MqttClient(brokerUrl, clientId, new MemoryPersistence());
            client.setCallback(new TmsMqttCallback(this.espData));
            client.connect();
            client.subscribe(topic);
            if (client.isConnected()) {
                System.out.println("MQTT connesso e sottoscritto al topic " + topic);
            }
        } catch (MqttException e) {
            System.err.println("Fatal error during MQTT initialization: " + e.getMessage());
        }
    }
}