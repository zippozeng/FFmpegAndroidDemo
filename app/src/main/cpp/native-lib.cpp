#include <jni.h>
#include <string>

#ifdef __cplusplus
extern "C"
{
#endif
#include "logger.h"
#include <libavutil/frame.h>
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif


#define TAG "ffmpeg-zippo"
#define LEDEBUG true

#define INBUF_SIZE 4096

static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize, char *filename) {
    leLogI(TAG, LEDEBUG, "pgm_save");
    FILE *f;
    int i;

    f = fopen(filename, "w");
    leLogI(TAG, LEDEBUG, "pgm open f:%d", f == NULL);
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    leLogI(TAG, LEDEBUG, "pgm fprintf");
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    leLogI(TAG, LEDEBUG, "pgm for earch");
    fclose(f);
    leLogI(TAG, LEDEBUG, "pgm_save end");
}

static int decode_write_frame(const char *outfilename, AVCodecContext *avctx,
                              AVFrame *frame, int *frame_count, AVPacket *pkt, int last) {
    int len, got_frame;
    char buf[1024];
    len = avcodec_decode_video2(avctx, frame, &got_frame, pkt);
    leLogI(TAG, LEDEBUG, "decode_write_frame len:%d", len);
    if (len < 0) {
        leLogE(TAG, LEDEBUG, "Error while decoding frame %d\n", *frame_count);
        return len;
    }
    if (got_frame) {
        leLogI(TAG, LEDEBUG, "Saving %sframe %3d\n", last ? "last " : "", *frame_count);
        fflush(stdout);

        /* the picture is allocated by the decoder, no need to free it */
        snprintf(buf, sizeof(buf), "%s-%d", outfilename, *frame_count);
        pgm_save(frame->data[0], frame->linesize[0],
                 frame->width, frame->height, buf);
        (*frame_count)++;
    }
    if (pkt->data) {
        pkt->size -= len;
        pkt->data += len;
    }
    return 0;
}

extern "C" JNIEXPORT jstring JNICALL
stringFromJNI(JNIEnv *env, jobject /* this */,
              jstring joutfilename, jstring jfilename) {
    const char *outfilename = env->GetStringUTFChars(joutfilename, NULL);
    const char *filename = env->GetStringUTFChars(jfilename, NULL);
    leLogI(TAG, LEDEBUG, "outfilename:%s", outfilename);
    leLogI(TAG, LEDEBUG, "filename:%s", filename);

    const AVCodec *codec;
    AVCodecContext *context = NULL;
    int frame_count;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    AVPacket avpkt;

    avcodec_register_all();
    av_log_set_callback(ffp_log_callback_report);
    av_init_packet(&avpkt);

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    /* find the MPEG-1 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        leLogI(TAG, LEDEBUG, "Codec not found\n");
        exit(1);
    }

    context = avcodec_alloc_context3(codec);
    if (!context) {
        leLogI(TAG, LEDEBUG, "Could not allocate video codec context\n");
        exit(1);
    }
//    if (avcodec_parameters_to_context(context, NULL) < 0) {
//        leLogI(TAG, LEDEBUG, "[%s   %d]can't copy decoder context \n", __FUNCTION__, __LINE__);
//        exit(1);
//    } else {
//        leLogI(TAG, LEDEBUG, "[%s   %d]copy decoder context success\n", __FUNCTION__, __LINE__);
//    }
    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED)
        context->flags |= AV_CODEC_FLAG_TRUNCATED; // we do not send complete frames

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open2(context, codec, NULL) < 0) {
        leLogI(TAG, LEDEBUG, "Could not open codec\n");
        exit(1);
    }

    f = fopen(filename, "rb");
    if (!f) {
        leLogI(TAG, LEDEBUG, "Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        leLogI(TAG, LEDEBUG, "Could not allocate video frame\n");
        exit(1);
    }

    frame_count = 0;
    for (;;) {
        avpkt.size = fread(inbuf, 1, INBUF_SIZE, f);
        if (avpkt.size == 0) {
            break;
        }

        /* NOTE1: some codecs are stream based (mpegvideo, mpegaudio)
           and this is the only     method to use them because you cannot
           know the compressed data size before analysing it.

           BUT some other codecs (msmpeg4, mpeg4) are inherently frame
           based, so you must call them with all the data for one
           frame exactly. You must also initialize 'width' and
           'height' before initializing them. */

        /* NOTE2: some codecs allow the raw parameters (frame size,
           sample rate) to be changed at any frame. We handle this, so
           you should also take care of it */

        /* here, we use a stream based decoder (mpeg1video), so we
           feed decoder and see if it could decode a frame */
        avpkt.data = inbuf;
        leLogI(TAG, LEDEBUG, "size:%d", avpkt.size);
        while (avpkt.size > 0) {
            if (decode_write_frame(outfilename, context, frame, &frame_count, &avpkt, 0) < 0) {
                exit(1);
            }
        }
    }

    /* Some codecs, such as MPEG, transmit the I- and P-frame with a
       latency of one frame. You must do the following to have a
       chance to get the last frame of the video. */
    avpkt.data = NULL;
    avpkt.size = 0;
    decode_write_frame(outfilename, context, frame, &frame_count, &avpkt, 1);

    fclose(f);

    avcodec_free_context(&context);
    av_frame_free(&frame);

    return env->NewStringUTF(NULL);
}

static const JNINativeMethod gMethods[] = {
        {"stringFromJNI", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (jstring *) stringFromJNI}
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
    if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0]))) {
        leLogI(TAG, LEDEBUG, "native", "register native method failed!\n");
        return -1;
    }
    return JNI_VERSION_1_4;
}