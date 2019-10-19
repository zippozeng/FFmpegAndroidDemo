package com.zippo.ffmpegplayer;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

/**
 * Created by huizai on 2017/11/24.
 */

public class GlView extends GLSurfaceView {

    GLRenderer renderer;

    public GlView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        renderer = new GLRenderer();
        setRenderer(renderer);
    }
}
