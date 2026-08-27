package mqtt;

import org.eclipse.paho.client.mqttv3.MqttCallback;

public class TmsMqttCallback implements MqttCallback {

    private final EspData espData;

    public TmsMqttCallback(final EspData espData) {
        this.espData = espData;
    }

    @Override
    public void connectionLost(Throwable cause) {
        System.out.println("Connection to MQTT broker lost: " + cause.getMessage());
    }

    @Override
    public void messageArrived(String topic, org.eclipse.paho.client.mqttv3.MqttMessage message) throws Exception {
        String payload = new String(message.getPayload());
        Double waterLevel = Double.parseDouble(payload);
        espData.setWaterLevel(waterLevel);
    }

    @Override
    public void deliveryComplete(org.eclipse.paho.client.mqttv3.IMqttDeliveryToken token) {
        // Not used for subscriptions
    }
}