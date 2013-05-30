package com.gloops.androidchatclient;

import android.os.Bundle;
import android.preference.PreferenceActivity;

public class ChatPreferenceActivity extends PreferenceActivity {
  @Override
  public void onCreate(Bundle savedInstanceState) {
	  // TODO implement preferences
      super.onCreate(savedInstanceState);
      addPreferencesFromResource(R.xml.preferences);
  }
} 