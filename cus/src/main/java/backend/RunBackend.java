package backend;

import io.vertx.core.Vertx;
import mqtt.EspData;
import mqtt.TmsMqttCommunicator;
import serialcommunication.ArduinoData;
import serialcommunication.SerialCommChannel;
import serialcommunication.SerialListener;

public class RunBackend {

    public static void main(String[] args) {
        try {
            final SerialCommChannel serialChannel = new SerialCommChannel("/dev/ttyACM0", 115200);
            final EspData espData = new EspData(0);
            final ArduinoData arduinoData = new ArduinoData();
            final Cus cus = new Cus(serialChannel, espData, arduinoData);
            final DataService dataService = new DataService(8080, espData, arduinoData);
            final SerialListener serialListener = new SerialListener(serialChannel, arduinoData);
            final TmsMqttCommunicator tmsMqttCommunicator = new TmsMqttCommunicator("tcp://mqtt-dashboard.com:1883", "waterData", espData);
            Vertx vertx = Vertx.vertx();
            serialListener.start();
            tmsMqttCommunicator.start();
            vertx.deployVerticle(dataService);
            cus.run();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
