package serialcommunication;

import backend.CusState;

public class SerialListener extends Thread {

	SerialCommChannel channel;

	private volatile boolean stopped = false;

	private final ArduinoData arduinoData;

	public SerialListener(final SerialCommChannel channel, final ArduinoData arduinoData) throws Exception {
		this.channel = channel;
		this.arduinoData = arduinoData;
	}

	public void run() {
		while (!stopped) {
			try {
				String msg = channel.receiveMsg();

				String[] parts = msg.split("\\|");
				if (parts[0].startsWith("MODE:")) {
					arduinoData.setState(CusState.valueOf(parts[0].substring(5)));
				}
				if (parts[1].startsWith("VALVE:")) {
					arduinoData.setValve(Double.parseDouble(parts[1].substring(6)));
				}
			} catch (Exception ex) {
				if (!stopped) {
					ex.printStackTrace();
				}
			}
		}
	}

	// Stop the monitoring agent thread
	public void stopAgent() {
		stopped = true;
		this.interrupt();
	}

}