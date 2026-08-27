package serialcommunication;

import java.util.function.Consumer;

/**
 * Simple interface for an async msg communication channel
 * @author aricci
 *
 */
public interface CommChannel {
	
	/**
	 * Send a message represented by a string (without new line).
	 * 
	 * Asynchronous model.
	 * 
	 * @param msg
	 */
	void sendMsg(String msg);
	
	/**
	 * To receive a message. 
	 * 
	 * Blocking behaviour.
	 */
	String receiveMsg() throws InterruptedException;

	/**
	 * To check if a message is available.
	 * 
	 * @return
	 */
	boolean isMsgAvailable();

	/**
	 * Register an asynchronous listener for incoming messages.
	 * 
	 * @param listener
	 */
	void registerListener(Consumer<String> listener);

	/**
	 * Close the communication channel.
	 */
	void close();

}