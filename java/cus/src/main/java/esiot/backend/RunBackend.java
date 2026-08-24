package esiot.backend;

import io.vertx.core.Vertx;
import io.vertx.mqtt.MqttClient;
import serialcommunication.CommChannel;
import serialcommunication.SerialCommChannel;

public class RunBackend {
	static final int PORT = 8080;
	
	public static void main(String[] args) {		
		Vertx vertx = Vertx.vertx();
		
		System.out.println("Starting CUS Backend...");
		
		// 1. Initialize DataService (HTTP Server & WebSocket/API for DBS)
		DataService service = new DataService(PORT);
		vertx.deployVerticle(service);
		
		// 2. Initialize Serial Communication with WCS (Arduino)
		CommChannel serialChannel = null;
		try {
		    // Assumes Arduino is connected to /dev/ttyACM0, fallback to COM3 for Windows
		    String os = System.getProperty("os.name").toLowerCase();
		    String port = os.contains("win") ? "COM3" : "/dev/ttyACM0";
		    serialChannel = new SerialCommChannel(port, 9600);
		    System.out.println("Serial connected on " + port);
		} catch (Exception e) {
		    System.out.println("Serial port not found or error: " + e.getMessage());
		    System.out.println("Running in headless/simulated mode without Arduino.");
		}
		
		// 3. Initialize Control Logic
		CusLogic logic = new CusLogic(serialChannel, service);
		service.setCusLogic(logic);
		
		// 4. Read from Serial asynchronously (Blocking take() on a separate thread)
		if (serialChannel != null) {
		    final CommChannel sc = serialChannel;
		    new Thread(() -> {
		        while(true) {
		            try {
		                String msg = sc.receiveMsg();
		                logic.onWcsMessage(msg);
		            } catch (InterruptedException e) {
		                break;
		            }
		        }
		    }).start();
		}
		
		// 5. Connect to MQTT Broker to receive data from TMS (ESP32)
		MqttClient mqttClient = MqttClient.create(vertx);
		mqttClient.connect(1883, "broker.mqtt-dashboard.com", s -> {
		    if (s.succeeded()) {
		        System.out.println("Connected to MQTT Broker");
		        mqttClient.subscribe("waterData", 1); // Subscribes to the topic defined in TMS config.h
		        mqttClient.publishHandler(msg -> {
		            if (msg.topicName().equals("waterData")) {
		                try {
		                    String payload = msg.payload().toString();
		                    double waterLevel = Double.parseDouble(payload);
		                    System.out.println("[MQTT] Received water level: " + waterLevel);
		                    logic.onRainwaterLevel(waterLevel);
		                } catch (Exception e) {
		                    System.out.println("[MQTT] Parse error: " + e.getMessage());
		                }
		            }
		        });
		    } else {
		        System.out.println("Failed to connect to MQTT broker");
		    }
		});
	}
}
