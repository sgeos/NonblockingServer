/***************************************************************
 *
 * SocketWriter.java
 * 
 * This module is used to write to a socket in a worker thread.
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
