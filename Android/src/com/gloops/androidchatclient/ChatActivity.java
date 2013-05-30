/***************************************************************
 *
 * ChatActivity.java
 * 
 * The main module for a simple Android chat client.
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

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class ChatActivity extends Activity implements EditTextEnterListenerCallback, SocketInitializerCallback, SocketReaderCallback {
	private static final String TAG = "ChatActivity";
	
	protected String       host;
	protected int          port;
	protected Socket       socket;
	protected SocketReader socketReader;
	protected Thread       socketReaderThread;
	protected String       username;
	protected Boolean      isLoginComplete;

	@Override
	protected void onCreate(Bundle pSavedInstanceState) {
		super.onCreate(pSavedInstanceState);
		setContentView(R.layout.activity_chat);
		
		// initialize layout
		clearText();
		clearInput();
		logout();
		
		// respond to enter key
		EditText chatEditText                            = (EditText) findViewById(R.id.chatEditText);
		TextView.OnEditorActionListener enterKeyListener = new EditTextEnterListener(this);
		chatEditText.setOnEditorActionListener(enterKeyListener);
		
		// maintain focus
		View.OnFocusChangeListener autoFocus = new AutoFocusChangeListener(this);
		chatEditText.setOnFocusChangeListener(autoFocus);
	}

	@Override
	protected void onStart() {
		// call the superclass method first
		super.onStart();
		focusInput();
		openSocket("192.168.5.169", 51717);
	}

	@Override
	protected void onStop() {
		// call the superclass method first
		super.onStop();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu pMenu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.chat, pMenu);
		return true;
	}

	private void openSocket(String pHost, int pPort)
	{
		host = pHost;
		port = pPort;
		appendText("Connecting...");
		SocketInitializer socketInitializer = new SocketInitializer(pHost, pPort, this);
		socketInitializer.execute();
	}
	
	// SocketInitializerCallback / SocketReaderCallback
	public void onIOException(IOException pIOException)
	{
		Log.e(TAG,"Socket IO fatal error.", pIOException);
		finish();
	}

	// SocketInitializerCallback
	public void onUnknownHostException(UnknownHostException pUnknownHostException)
	{
		Log.e(TAG,"Uknown host fatal error.", pUnknownHostException);
		finish();
	}

	// SocketInitializerCallback
	public void initSocket(Socket pSocket)
	{
		try {
			socket             = pSocket;
			socketReader       = new SocketReader(socket, this, this);
			socketReaderThread = new Thread(socketReader);
			socketReaderThread.start();
			
			appendText("Connected.");
			appendText(String.format("Host %s", host));
			appendText(String.format("Port %d", port));
		} catch (IOException exception) {
			onIOException(exception);
		}
	}

	// SocketReaderCallback
	public void receiveMessage(String pMessage)
	{
		if (!pMessage.matches(getString(R.string.nullString))) {
			appendText(pMessage);
		}
	}

	// EditTextEnterListenerCallback
	public void onEnter(TextView pTextView, int pActionId, KeyEvent pEvent)
	{
		onChatEvent(pTextView);
	}	

	public void sendMessage(String pMessage)
	{
		try {
			String[]     messageList  = {pMessage};
			SocketWriter socketWriter = new SocketWriter(socket);
			socketWriter.execute(messageList);
		} catch (IOException exception) {
			onIOException(exception);
		}
	}
	
	protected void relabelButton(String pLabel) {
		// relabel button
		Button   chatButton   = (Button)   findViewById(R.id.chatButton);
		chatButton.setText(pLabel);
		
		// use same label for enter key in edit text
		EditText chatEditText = (EditText) findViewById(R.id.chatEditText);
		chatEditText.setImeActionLabel(pLabel, KeyEvent.KEYCODE_ENTER);
	}

	protected void setHint(String pHint) {
		EditText chatEditText = (EditText) findViewById(R.id.chatEditText);
		chatEditText.setHint(pHint);
	}

	protected void setUI(String pLabel, String pHint) {
		relabelButton(pLabel);
		setHint(pHint);
	}

	protected void setText(String pText) {
		TextView chatTextView = (TextView) findViewById(R.id.chatTextView);
		chatTextView.setText(pText);
	}

	protected void clearText() {
		String text = getString(R.string.nullString);
		setText(text);
	}

	protected void clearInput() {
		EditText chatEditText = (EditText) findViewById(R.id.chatEditText);
		String   text = getString(R.string.nullString);
		chatEditText.setText(text);
	}

	protected void focusInput() {
		// reset focus
		EditText chatEditText = (EditText) findViewById(R.id.chatEditText);
		chatEditText.clearFocus();
		chatEditText.requestFocus();
		
		// show keyboard
    	InputMethodManager inputMethodManager = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
    	inputMethodManager.showSoftInput(chatEditText, InputMethodManager.SHOW_IMPLICIT);

	}

	protected void appendText(String pText) {
		TextView chatTextView = (TextView) findViewById(R.id.chatTextView);
		String   text = pText + "\n" + chatTextView.getText().toString();
		chatTextView.setText(text);
	}

	protected void logout() {
		String label = getString(R.string.buttonJoin);
		String hint  = getString(R.string.hintJoin);
		setUI(label, hint);
		isLoginComplete = false;
		username        = getString(R.string.defaultUsername);
	}

	protected void login(String pUsername) {
		String label = getString(R.string.buttonSend);
		String hint  = getString(R.string.hintSend);
		setUI(label, hint);
		isLoginComplete = true;
		username        = pUsername;
		String message  = String.format("/iam/0/%s", pUsername);
		sendMessage(message);
	}

	protected void sendChatMessage(String pMessage) {
		String message = String.format("/msg/0/%s", pMessage);
		sendMessage(message);
	}

	/*** called when the user presses the button or enter
     */
	public void onChatEvent(View pView) {
		EditText chatEditText = (EditText) findViewById(R.id.chatEditText);
		String   text         = chatEditText.getText().toString();
		if (!text.matches(getString(R.string.nullString))) {
			if (isLoginComplete) {
				sendChatMessage(text);
			} else {
				login(text);
			}
		}
		clearInput();
		focusInput();
	}
}
