package com.gloops.androidchatclient;

import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import android.os.AsyncTask;

public class SocketInitializer extends AsyncTask<Void, Void, Void> {
	String                    host;
	int                       port;
	SocketInitializerCallback callbackObject;

	public SocketInitializer(String pHost, int pPort, SocketInitializerCallback pCallbackObject)
	{
		super();
		host                 = pHost;
		port                 = pPort;
		callbackObject       = pCallbackObject;
	}
	
	@Override
	protected Void doInBackground(Void... params) {
		try {
			Socket socket = new Socket(host, port);
			callbackObject.initSocket(socket);
		} catch (UnknownHostException exception) {
			callbackObject.onUnknownHostException(exception);
		} catch (IOException exception) {
			callbackObject.onIOException(exception);
		}
		return null;
	}
}
