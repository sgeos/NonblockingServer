/***************************************************************
 *
 * SocketInitializer.java
 * 
 * A module that creates a socket in a worker thread.
 * SocketInitializerCallback is used to pass the socket back to
 * the main activity.
 *
 ***************************************************************
 *
 * This software was written in 2013 by the following author(s):
 * Brendan A R Sechter <bsechter@sennue.com>
 *
 * To the extent possible under law, the author(s) have
 * dedicated all copyright and related and neighboring rights
 * to this software to the public domain worldwide. This
 * software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain
 * Dedication along with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 *
 * Please release derivative works under the terms of the CC0
 * Public Domain Dedication.
 *
 ***************************************************************/

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
