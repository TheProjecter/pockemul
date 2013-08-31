/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.qtproject.pockemul;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Context;
import android.content.ContentResolver;
import android.content.res.Resources;
import android.os.Looper;
import android.os.Vibrator;
import android.util.Log;
import android.provider.Settings;
import android.provider.Settings.System;
import android.util.Log;

import java.util.concurrent.Semaphore;

import org.qtproject.qt5.android.bindings.QtActivity;


public class Pockemul {

    /** If no pattern was found, vibrate for a small amount of time. */
    private static final long DURATION = 10;  // millisec.
    /** Play the haptic pattern only once. */
    private static final int NO_REPEAT = -1;

    private static final String TAG = "Pockemul";
    private Context mContext;
    private long[] mHapticPattern;
    private Vibrator mVibrator;

    private boolean mEnabled;
    private Settings.System mSystemSettings;
    private ContentResolver mContentResolver;
    private boolean mSettingEnabled;



    public void vibrate() {
    Log.i("Qt", "OKOKOKOKOKOKOKOKOKOKOKOKOk");

    final String msg = "QT_USE_ANDROID_NATIVE_STYLE=1\t";
    final Boolean result = false;

    QtActivity.getQtActivityInstance().runOnUiThread(new Runnable()
    {
        public void run()
        {

Log.i("Qt", "1111111111111111111111111111111");
            AlertDialog.Builder builder = new AlertDialog.Builder(QtActivity.getQtActivityInstance());
            builder.setMessage(msg)
                   .setCancelable(false)
                   .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                       public void onClick(DialogInterface dialog, int id) {
                            /*Pockemul.this.finish();*/

                       }
                   })
                   .setNegativeButton("No", new DialogInterface.OnClickListener() {
                       public void onClick(DialogInterface dialog, int id) {
                            dialog.cancel();
                       }
                   });
                   Log.i("Qt", "222222222222222222222222222222222");
            AlertDialog alert = builder.create();

            Log.i("Qt", "3333333333333333333333333333333333");
            alert.show();
            Log.i("Qt", "4444444444444444444444444444444444444444'");

            }
       });

    }

private int pressedButtonID;
private String dialogMsg;
private int nbButtons;

    private final Semaphore dialogSemaphore = new Semaphore(0, true);
    private final Runnable mMyDialog = new Runnable()
    {
        public void run()
        {
            AlertDialog errorDialog = new AlertDialog.Builder( QtActivity.getQtActivityInstance() ).create();
            errorDialog.setMessage(dialogMsg);
            errorDialog.setButton("Yes", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    pressedButtonID = 1;
                    dialogSemaphore.release();
                    }
                });
            errorDialog.setButton2("No", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    pressedButtonID = 2;
                    dialogSemaphore.release();
                    }
                });
             if (nbButtons==3) {
                errorDialog.setButton3("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        pressedButtonID = 3;
                        dialogSemaphore.release();
                        }
                    });
                }
            errorDialog.setCancelable(false);
            errorDialog.show();
        }
    };

    public int ShowMyModalDialog(String msg,int nb)  //should be called from non-UI thread
    {
        pressedButtonID=-1;
        dialogMsg = msg;
        nbButtons = nb;
        Log.i("Qt", "4444444444444444444444444444444444444444");
        QtActivity.getQtActivityInstance().runOnUiThread(mMyDialog);
        try
        {
            dialogSemaphore.acquire();
        }
        catch (InterruptedException e)
        {
        }
        Vibrate();
        return pressedButtonID;
    }

    public void Vibrate()
    {
    // Get instance of the Vibrator
     Vibrator vibrator = (Vibrator) QtActivity.getQtActivityInstance().getSystemService(Context.VIBRATOR_SERVICE);
     // Vibrate for 50 milliseconds
     vibrator.vibrate(50);
    }

}

