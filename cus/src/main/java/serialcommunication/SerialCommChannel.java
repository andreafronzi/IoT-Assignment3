package serialcommunication;

import java.util.concurrent.*;
import java.util.function.Consumer;
import jssc.*;

/**
 * Comm channel implementation based on serial port.
 */
public class SerialCommChannel implements CommChannel, SerialPortEventListener {

	private SerialPort serialPort;
	private BlockingQueue<String> queue;
	private StringBuffer currentMsg = new StringBuffer("");
	private Consumer<String> listener;
	
	public SerialCommChannel(String port, int rate) throws Exception {
		queue = new ArrayBlockingQueue<String>(100);

		serialPort = new SerialPort(port);
		serialPort.openPort();

		serialPort.setParams(rate,
								 SerialPort.DATABITS_8,
								 SerialPort.STOPBITS_1,
								 SerialPort.PARITY_NONE);

		serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_RTSCTS_IN | 
										  SerialPort.FLOWCONTROL_RTSCTS_OUT);

		// serialPort.addEventListener(this, SerialPort.MASK_RXCHAR);
		serialPort.addEventListener(this);
	}

	@Override
	public void registerListener(Consumer<String> listener) {
		this.listener = listener;
	}

	@Override
	public void sendMsg(String msg) {
		if (serialPort == null || !serialPort.isOpened()) {
			System.err.println("Error: Cannot send message, serial port is not open.");
			return;
		}
		char[] array = (msg+"\n").toCharArray();
		byte[] bytes = new byte[array.length];
		for (int i = 0; i < array.length; i++){
			bytes[i] = (byte) array[i];
		}
		try {
			synchronized (serialPort) {
				serialPort.writeBytes(bytes);
			}
		} catch(Exception ex){
			ex.printStackTrace();
		}
	}

	@Override
	public String receiveMsg() throws InterruptedException {
		return queue.take();
	}

	@Override
	public boolean isMsgAvailable() {
		return !queue.isEmpty();
	}

	/**
	 * This should be called when you stop using the port.
	 * This will prevent port locking on platforms like Linux.
	 */
	public void close() {
		try {
			if (serialPort != null && serialPort.isOpened()) {
				serialPort.removeEventListener();
				serialPort.closePort();
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}


	public void serialEvent(SerialPortEvent event) {
		/* if there are bytes received in the input buffer */
		if (event.isRXCHAR()) {
            try {
            		String msg = serialPort.readString(event.getEventValue());
            		
            		msg = msg.replaceAll("\r", "");
            		
            		currentMsg.append(msg);
            		
            		boolean goAhead = true;
            		
        			while(goAhead) {
        				String msg2 = currentMsg.toString();
        				int index = msg2.indexOf("\n");
            			if (index >= 0) {
            				String completeMsg = msg2.substring(0, index);
            				queue.put(completeMsg);
            				if (listener != null) {
            					listener.accept(completeMsg);
            				}
            				currentMsg = new StringBuffer("");
            				if (index + 1 < msg2.length()) {
            					currentMsg.append(msg2.substring(index + 1)); 
            				}
            			} else {
            				goAhead = false;
            			}
        			}
        			
            } catch (Exception ex) {
            		ex.printStackTrace();
                System.out.println("Error in receiving string from COM-port: " + ex);
            }
        }
	}
}