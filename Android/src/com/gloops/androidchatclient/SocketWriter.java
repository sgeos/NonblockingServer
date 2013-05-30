package com.gloops.androidchatclient;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;

import android.os.AsyncTask;

public class SocketWriter extends AsyncTask<String, Void, Void> {
	protected PrintWriter  socketWriter;

	public SocketWriter(Socket pSocket) throws IOException {
		super();
		socketWriter = new PrintWriter(pSocket.getOutputStream());
	}
	
	@Override
	protected Void doInBackground(String... pMessageList) {
        int count = pMessageList.length;
        for (int i = 0; i < count; i++) {
        	sendMessage(pMessageList[i]);
        }
        return null;
	}

	public void sendMessage(String pMessage)
	{
		socketWriter.println(pMessage);
		socketWriter.flush();
	}
}
