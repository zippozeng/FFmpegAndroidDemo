package com.zippo.ffmpegplayer;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.widget.TextView;

import java.io.File;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED || ContextCompat.checkSelfPermission(this,
                Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 0);
        } else {
            //
        }
        // Example of a call to a native method
        final TextView tv = findViewById(R.id.sample_text);
        AssetsUtil instance = AssetsUtil.getInstance(getApplicationContext());
        instance.setFileOperateCallback(new AssetsUtil.FileOperateCallback() {

            @Override
            public void onSuccess() {
                File file = new File(Environment.getExternalStorageDirectory(), "1/ios10_3.264");
                File outFile = new File(file.getParent(), "test.yuv");
                if (file.exists()) {
                    tv.setText(stringFromJNI(outFile.getAbsolutePath(), file.getAbsolutePath()));
                }
            }

            @Override
            public void onFailed(String error) {

            }
        });
        instance.copyAssetsToSD("file", getCacheDir().getPath());

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI(String outFileName, String fileName);
}
