package com.kdab.flowpomodoro;

import android.view.WindowManager;
import org.qtproject.qt5.android.bindings.QtActivity;

public class MainActivity extends QtActivity {
    public void setKeepScreenOn(final boolean on) {
        runOnUiThread( new Runnable() {
            @Override
            public void run() {
                try {
                    if (on)
                        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                    else
                        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }
}
