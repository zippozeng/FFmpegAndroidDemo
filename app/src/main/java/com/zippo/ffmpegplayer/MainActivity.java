package com.zippo.ffmpegplayer;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.os.Build;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.util.Range;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;

import androidx.appcompat.app.AppCompatActivity;

import static android.media.MediaCodecList.ALL_CODECS;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String TAG = "MainActivity";
    private TextView tv;
    private SurfaceView mSurfaceView;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpeg-player");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
        initSurface();

        try {
            MediaCodec mediaCodec = MediaCodec.createByCodecName(getAVCName());
            MediaCodecInfo codecInfo;
            if (Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR2) {
                codecInfo = mediaCodec.getCodecInfo();
                String[] supportedTypes = codecInfo.getSupportedTypes();
                MediaCodecInfo.CodecCapabilities capabilities = codecInfo.getCapabilitiesForType(supportedTypes[0]);

                if (Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                    MediaCodecInfo.VideoCapabilities videoCapabilities = capabilities.getVideoCapabilities();
                    Range<Integer> supportedWidths = videoCapabilities.getSupportedWidths();
                    Range<Integer> supportedHeights = videoCapabilities.getSupportedHeights();
                    Range<Integer> supportedFrameRates = videoCapabilities.getSupportedFrameRates();
                    int widthAlignment = videoCapabilities.getWidthAlignment();
                    int heightAlignment = videoCapabilities.getHeightAlignment();
                    int supportedLowerWidth = supportedWidths.getLower().intValue();
                    int supportedUpperWidth = supportedWidths.getUpper().intValue();
                    int supportedLowerHeight = supportedHeights.getLower().intValue();
                    int supportedUpperHeight = supportedHeights.getUpper().intValue();
                    int supportedLowerFrameRate = supportedFrameRates.getLower().intValue();
                    int supportedUpperFrameRate = supportedFrameRates.getUpper().intValue();
                    Log.i(TAG, "supportedLowerWidth:" + supportedLowerWidth + "  supportedUpperWidth:" + supportedUpperWidth
                            + "  supportedLowerHeight:" + supportedLowerHeight + "  supportedUpperHeight:" + supportedUpperHeight
                            + "  supportedLowerFrameRate:" + supportedLowerFrameRate + "  supportedUpperFrameRate:" + supportedUpperFrameRate
                            + " widthAlignment:" + widthAlignment + "  heightAlignment:" + heightAlignment);
                }

                Log.d(TAG, "supportedTypes:" + Arrays.toString(supportedTypes));
            }
        } catch (Exception e) {
            Log.w(TAG, e);
        }
    }

    private void initView() {
        // Example of a call to a native method
        tv = findViewById(R.id.sample_text);
        findViewById(R.id.btn).setOnClickListener(this);
        mSurfaceView = findViewById(R.id.surface_view);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {

            @Override
            public void surfaceCreated(SurfaceHolder holder) {
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }

    private void initSurface() {
        mSurfaceView.getHolder().getSurface();
    }

    public static String getAVCName() {
        if (Build.VERSION.SDK_INT >= 21) {
            // Android 5.0 LOLLIPOP
            try {
                MediaCodecList codecList = new MediaCodecList(ALL_CODECS);
                MediaCodecInfo[] codecs = codecList.getCodecInfos();
                for (int i = 0; i < codecs.length; i++) {
                    String name = getAVCName(codecs[i]);
                    if (!TextUtils.isEmpty(name)) {
                        return name;
                    }
                }
            } catch (Exception e) {
                Log.w(TAG, e);
            } catch (UnsatisfiedLinkError error) {
                Log.w(TAG, error);
            }
        } else if (Build.VERSION.SDK_INT >= 16) {
            // Android 4.1、4.1.1 JELLY_BEAN
            int count = MediaCodecList.getCodecCount();
            for (int i = 0; i < count; i++) {
                String name = getAVCName(MediaCodecList.getCodecInfoAt(i));
                if (!TextUtils.isEmpty(name)) {
                    return name;
                }
            }
        }
        return "";
    }

    private static String getAVCName(MediaCodecInfo codec) {
        if (codec.isEncoder()) {
            return "";
        }
        String type[] = codec.getSupportedTypes();
        for (int j = 0; j < type.length; j++) {
            if (type[j].equals("video/avc")) {
                return codec.getName();
            }
        }
        return "";
    }

    private void init() {
        final File outFile = new File(getApplicationContext().getExternalCacheDir(), "test.yuv");
        Log.d(TAG, "init outFile path:" + outFile.getAbsolutePath());
        if (!outFile.exists()) {
            try {
                boolean newFile = outFile.createNewFile();
                Log.d(TAG, "init create test.yuv:" + newFile);
            } catch (IOException e) {
                e.printStackTrace();
                Toast.makeText(MainActivity.this, "create new file error", Toast.LENGTH_SHORT).show();
            }
        }
        final File file = new File(getApplicationContext().getExternalCacheDir(), "output.h264");
        if (!file.exists()) {
            AssetsUtil instance = AssetsUtil.getInstance(getApplicationContext());
            instance.setFileOperateCallback(new AssetsUtil.FileOperateCallback() {

                @Override
                public void onSuccess() {
                    Log.i(TAG, "output.h264 file exitst:" + file.exists());
                    if (file.exists()) {
                        tv.setText(stringFromJNI(outFile.getAbsolutePath(), file.getAbsolutePath(), mSurfaceView));
                    } else {
                        Toast.makeText(MainActivity.this, "copy file failure", Toast.LENGTH_SHORT).show();
                    }
                }

                @Override
                public void onFailed(String error) {
                    Toast.makeText(MainActivity.this, "copy file error", Toast.LENGTH_SHORT).show();
                }
            });
            instance.copyAssetsToSD("file", "");
        } else {
            tv.setText(stringFromJNI(outFile.getAbsolutePath(), file.getAbsolutePath(), mSurfaceView));
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI(String outFileName, String fileName, SurfaceView surfaceView);

    public native int _setSurface(Surface surface);

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btn:
                init();
                break;
        }
    }

}
