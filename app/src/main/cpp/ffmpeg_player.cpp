#include <jni.h>
#include <string>
#include "H264_decoder.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "logger.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
}
#endif


#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif


#define TAG "ffmpeg-zippo"
#define LEDEBUG true

jobject jobjSurface;

extern "C" JNIEXPORT jint JNICALL
set_surface(JNIEnv *env, jobject /* this */, jobject object) {
    leLogD(TAG, LEDEBUG, "set_surface");
    if (jobjSurface != NULL) {
        env->DeleteGlobalRef(jobjSurface);
    }
    jobjSurface = env->NewGlobalRef(object);
    return 0;
}

typedef struct file_info {
    char *outfilename;
    char *filename;

} file_info;

pthread_t thread;

void *start_decode(void *args) {
    file_info *pFileInfo = (file_info *) args;
    H264Decoder h264Decoder = H264Decoder();
    h264Decoder.init();
    leLogD(TAG, LEDEBUG, "isNull:%d", pFileInfo == NULL);
    h264Decoder.decode(pFileInfo->outfilename, pFileInfo->filename);
}

extern "C" JNIEXPORT jstring JNICALL
stringFromJNI(JNIEnv *env, jobject /* this */,
              jstring joutfilename, jstring jfilename, jobject surface) {
    const char *outfilename = env->GetStringUTFChars(joutfilename, NULL);
    const char *filename = env->GetStringUTFChars(jfilename, NULL);
    leLogI(TAG, LEDEBUG, "outfilename: %s,length:%d", outfilename, strlen(outfilename));
    leLogI(TAG, LEDEBUG, "filename:%s", filename);
//    file_info fileInfo;
//    strcpy(fileInfo.outfilename, outfilename);
//    strcpy(fileInfo.filename, filename);

//    pthread_create(&thread, NULL, start_decode, (void *) &fileInfo);
    //获取界面传下来的surface
    H264Decoder h264Decoder = H264Decoder();
    h264Decoder.init();
    h264Decoder.decode(outfilename, filename);

    return env->NewStringUTF("Zippo");
}

static const JNINativeMethod gMethods[] = {
        {"stringFromJNI", "(Ljava/lang/String;Ljava/lang/String;Landroid/view/SurfaceView;)Ljava/lang/String;", (jstring *) stringFromJNI},
        {"_setSurface",   "(Landroid/view/Surface;)I",                                (jint *) set_surface}
};

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {

    __android_log_print(ANDROID_LOG_INFO, "native", "Jni_OnLoad");
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) //从JavaVM获取JNIEnv，一般使用1.4的版本
        return -1;
    jclass clazz = env->FindClass("com/zippo/ffmpegplayer/MainActivity");
    if (!clazz) {
        leLogI(TAG, LEDEBUG, "cannot get class: com/zippo/ffmpegplayer/MainActivity");
        return -1;
    }
    if (env->RegisterNatives(clazz, gMethods, NELEM(gMethods))) {
        leLogI(TAG, LEDEBUG, "native", "register native method failed!\n");
        return -1;
    }
    return JNI_VERSION_1_4;
}