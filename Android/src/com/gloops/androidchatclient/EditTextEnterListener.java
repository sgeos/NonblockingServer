/***************************************************************
 *
 * EditTextEnterListener.java
 * 
 * A module that allows EditText objects to respond to the
 * enter key.  Implement the EditTextEnterListenerCallback
 * interface to define a response to the enter key.
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

import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.widget.TextView;

public class EditTextEnterListener implements android.widget.TextView.OnEditorActionListener {
	EditTextEnterListenerCallback callbackObject;
	
	public EditTextEnterListener(EditTextEnterListenerCallback pCallbackObject) {
		super();
		callbackObject = pCallbackObject;
	}
	
	@Override
	public boolean onEditorAction(TextView pTextView, int pActionId, KeyEvent pEvent) {
		boolean result = false;
		if (null == pEvent) {
			if ((EditorInfo.IME_ACTION_DONE == pActionId) || (EditorInfo.IME_ACTION_NEXT == pActionId)) {
				result = true;
			}
		} else if ((KeyEvent.ACTION_DOWN == pEvent.getAction())) {
			if ((EditorInfo.IME_NULL == pActionId) || (KeyEvent.KEYCODE_ENTER == pActionId)) {
				result = true;
			}
		}
		if (true == result) {
			callbackObject.onEnter(pTextView, pActionId, pEvent);
		}
		return result;
	}
}
