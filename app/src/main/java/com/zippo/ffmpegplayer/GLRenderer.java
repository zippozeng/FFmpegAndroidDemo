package com.zippo.ffmpegplayer;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by zippo on 2019-10-19.
 * Date: 2019-10-19
 * Time: 14:51
 */
public class GLRenderer implements GLSurfaceView.Renderer {

    public native void InitOpenGL();

    public native void OnViewportChanged(float width, float height);

    public native void RenderOneFrame();

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        InitOpenGL();
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        OnViewportChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        RenderOneFrame();
    }
}
