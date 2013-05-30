/***************************************************************
 *
 * SocketReader.java
 * 
 * SocketReader reads from a socket in a worker thread.
 * SocketReaderCallback is used to pass the received message
 * back to the main activity.
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
