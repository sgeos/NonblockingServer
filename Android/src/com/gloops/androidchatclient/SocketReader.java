package com.gloops.androidchatclient;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

import android.app.Activity;

public class SocketReader implements Runnable {
	BufferedReader       socketReader;
	SocketReaderCallback callbackObject;
	Activity             activity;

	public SocketReader(Socket pSocket, SocketReaderCallback pCallbackObject, Activity pActivity) throws IOException {
		super();
		InputStreamReader inputStreamReader = new InputStreamReader(pSocket.getInputStream());
		socketReader                        = new BufferedReader(inputStreamReader);
		callbackObject                      = pCallbackObject;
		activity                            = pActivity;
	}
	
	private void runLoop() throws IOException {
		String message;
		for (message = socketReader.readLine(); null != message; message = socketReader.readLine()) {
			SocketReaderUIBridge uiBridge = new SocketReaderUIBridge(callbackObject, message);
			activity.runOnUiThread(uiBridge);
		}
	}

	@Override
	public void run() {
		try {
			runLoop();
		} catch (IOException exception) {
			callbackObject.onIOException(exception);
		}
	}
}
