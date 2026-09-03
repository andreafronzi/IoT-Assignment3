package serialcommunication;

import backend.CusState;

public class SerialListener extends Thread {

    private final SerialCommChannel channel;
    private volatile boolean stopped = false;
    private final ArduinoData arduinoData;

    public SerialListener(final SerialCommChannel channel, final ArduinoData arduinoData) {
        this.channel = channel;
        this.arduinoData = arduinoData;
    }

    @Override
    public void run() {
        while (!stopped) {
            try {
                String msg = channel.receiveMsg();
                if (msg == null || msg.isEmpty()) continue;

                // 1. RICEZIONE TELEMETRIA (Heartbeat - es. "HEARTBEAT:MODE:MANUAL|VALVE:45.0")
                if (msg.startsWith("HEARTBEAT:")) {
                    String data = msg.substring(10); 
                    String[] parts = data.split("\\|");
                    
                    if (parts[0].startsWith("MODE:")) {
                        arduinoData.setTelemetryMode(CusState.valueOf(parts[0].substring(5)));
                    }
                    if (parts.length > 1 && parts[1].startsWith("VALVE:")) {
                        arduinoData.setTelemetryValve(Double.parseDouble(parts[1].substring(6)));
                    }
                } 
                // 2. RICEZIONE INTENTI (Richieste hardware)
                else if (msg.equals("INTENT:TOGGLE_MODE")) {
                    arduinoData.setIntentToggleMode(true);
                } 
                else if (msg.startsWith("INTENT:SET_VALVE:")) {
                    double val = Double.parseDouble(msg.substring(17));
                    arduinoData.setIntentValve(val);
                }
            } catch (Exception ex) {
                if (!stopped) {
                    ex.printStackTrace();
                }
            }
        }
    }

    public void stopAgent() {
        stopped = true;
        this.interrupt();
    }
}