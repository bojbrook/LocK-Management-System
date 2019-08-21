package com.cmpe123.lock_management_system.ucsclocks;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.util.Log;

/**
 * Created by bowen on 4/16/2018.
 */

public class AccountStorage {
  private static final String TAG = "AccountStorage";
  private static final String PREF_CRUZID = "cruzId";
  private static String sAccount = null;
  private static final Object sAccountLock = new Object();

  public static void SetAccount(Context c, String s) {
    synchronized(sAccountLock) {
      Log.i(TAG, "Setting cruzID: " + s);
      SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(c);
      prefs.edit().putString(PREF_CRUZID, s).commit();
      sAccount = s;
    }
  }

  public static String getAccount(Context context){
    synchronized (sAccountLock){
      if(sAccount == null){
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        String cruzID = prefs.getString(PREF_CRUZID,null);
        sAccount = cruzID;
      }
      return sAccount;
    }
  }


}
