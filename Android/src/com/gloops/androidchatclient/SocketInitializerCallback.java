package com.gloops.androidchatclient;

import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

public interface SocketInitializerCallback {
	public void initSocket(Socket pSocket);
	public void onIOException(IOException pIOException);
	public void onUnknownHostException(UnknownHostException pUnknownHostException);
}
