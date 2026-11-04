package com.libertyrecomp;

import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;
import org.libsdl.app.SDLActivity;

public class LibertyActivity extends SDLActivity {
    private static final String TAG = "LibertyActivity";

    // JNI Native Runtime Bindings
    public static native boolean nativeInitRuntime(String storageDir);
    public static native String nativeGetRuntimeStatus();
    public static native int nativeGetRuntimeState();
    public static native String nativeGetHardwareReportJson();

    static {
        try {
            System.loadLibrary("LibertyUniversalCore");
            Log.i(TAG, "Successfully loaded libLibertyUniversalCore.so");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "libLibertyUniversalCore.so could not be pre-loaded: " + e.getMessage());
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        String filesDir = getExternalFilesDir(null) != null ?
            getExternalFilesDir(null).getAbsolutePath() : getFilesDir().getAbsolutePath();

        try {
            boolean initialized = nativeInitRuntime(filesDir);
            String status = nativeGetRuntimeStatus();
            Log.i(TAG, "Liberty City Universal Runtime initialized: " + initialized + " | Status: " + status);
            Toast.makeText(this, status, Toast.LENGTH_LONG).show();
        } catch (Throwable t) {
            Log.e(TAG, "Error invoking native runtime: " + t.getMessage());
        }
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "LibertyUniversalCore"
        };
    }
}
