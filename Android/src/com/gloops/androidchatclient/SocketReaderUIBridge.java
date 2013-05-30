package com.gloops.androidchatclient;

public class SocketReaderUIBridge implements Runnable {
	SocketReaderCallback callbackObject;
	String               message;

	public SocketReaderUIBridge(SocketReaderCallback pCallbackObject, String pMessage) {
		super();
		callbackObject = pCallbackObject;
		message        = pMessage;
	}
	
    @Override
    public void run() {
		callbackObject.receiveMessage(message);
    }
}
