package com.zippo.ffmpegplayer;

import android.os.Message;
import android.widget.TextView;

/**
 * Created by zippo on 2019-10-19.
 * Date: 2019-10-19
 * Time: 10:24
 */
public class FFmpegPlayer {


    /*********************************************************
     * jni 接口
     */
    static {
        System.loadLibrary("ffmpeg-player");
        System.loadLibrary("ffmpeg");
    }

    public native int OpenFileWithUrl(String input);

    public native int ThreadTest(String input);

    public native int ReadPkt();

    public native int DecodeVideo();

    public native int DecodeAudio();

    public native int SetPlayRate(int playRate);

    public native void SetPlayAudioOrVideoRate(int playRate);

    TextView tv;
    GlView mGlView;
    public int number = 10;
    public int isExit = 0;
    String myStr = "this is string test";

    int OpenAVWithUrlAndView(String url, TextView view, GlView glView) {
        mGlView = glView;
        OpenFileWithUrl(url);
        ThreadTest(url);
        tv = view;
        new Thread(new ReadPktThread()).start();
        new Thread(new DecodeVideoThread()).start();
        return 0;
    }

    void setPlayRate(int playRate) {
        SetPlayRate(playRate);
    }

    void setPlayAudioOrVideoRate(int playRate) {
        SetPlayAudioOrVideoRate(playRate);
    }

    public class ReadPktThread implements Runnable {
        @Override
        public void run() {
            while (isExit == 0) {
                int ret = 0;
                ret = ReadPkt();
                if (ret == 0) {
                    DecodeAudio();
                } else if (ret == -2) {
                    continue;
                } else if (ret == -9) {
                    isExit = 1;
                } else {
                    continue;
                }
                Message message = Message.obtain();
                //  message.obj = pts;
                message.what = 0;
                // mHandler.sendMessage(message);
            }
        }
    }

    public class DecodeVideoThread implements Runnable {
        @Override
        public void run() {
            int pts;
            while (isExit == 0) {
                int ret = 0;
                ret = DecodeVideo();
                if (ret == 0) {
                    mGlView.requestRender();
                }
                Message message = Message.obtain();
                message.obj = ret;
                message.what = 1;
                //  mHandler.sendMessage(message);
            }
        }
    }

}
