package com.gloops.androidchatclient;

import java.io.IOException;

public interface SocketReaderCallback {
	public void receiveMessage(String pMessage);
	public void onIOException(IOException pIOException);
}
