//
// Created by Zippo on 2019-10-12.
//
#include "H264_decoder.h"

#define TAG "H264_decoder"
#define LEDEBUG true

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

int H264Decoder::init() {
    av_register_all();
    av_log_set_callback(ffp_log_callback_report);
    avformat_network_init();
    return 0;
}

int H264Decoder::decode(const char *outfilename, const char *filename) {
    leLogD(TAG, LEDEBUG, "decode outFileName:%s fileName:%s, outLength:%d", outfilename,
           filename, strlen(outfilename));
    AVFormatContext *pFormatContext;
    AVCodecContext *pCodecContext = NULL;
    AVCodec *pCodec;
    AVFrame *pFrame;
    AVFrame *pFrameYUV;
    AVPacket *pPacket;
    uint8_t *outBuffer;
    SwsContext *pSwsContext;
    FILE *yuvFilePath;

    // 1、初始化
    init();

    // 2、获取AVFormatContext
    pFormatContext = avformat_alloc_context();

    // 3、打开文件
    yuvFilePath = fopen(outfilename, "rb+");

    int error_code = avformat_open_input(&pFormatContext, filename, NULL, NULL) != 0;
    if (error_code) {
        leLogD(TAG, LEDEBUG, "Couldn't open input stream error_code:%s .\n",
               av_err2str(error_code));
        exit(EXIT_FAILURE);
    }
    // 4、获取文件信息
    if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
        leLogD(TAG, LEDEBUG, "Couldn't find stream information.");
        exit(EXIT_FAILURE);
    }

    // 5、获取视频的index
    int videoIndex = -1;
    for (int i = 0; i < pFormatContext->nb_streams; ++i) {
        if (pFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            break;
        }
    }
    if (videoIndex == -1) {
        leLogD(TAG, LEDEBUG, "Didn't find a video stream.");
        exit(EXIT_FAILURE);
    }

    // 6、获取解码器并打开
    pCodecContext = avcodec_alloc_context3(NULL);
    if (avcodec_parameters_to_context(pCodecContext,
                                      pFormatContext->streams[videoIndex]->codecpar) < 0) {
        leLogD(TAG, LEDEBUG, "Didn't parameters to contex.");
        exit(EXIT_FAILURE);
    }
    pCodec = avcodec_find_decoder(pCodecContext->codec_id);
    if (pCodec == NULL) {
        leLogD(TAG, LEDEBUG, "Codec not found.");
        exit(EXIT_FAILURE);
    }
    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) {//打开解码器
        leLogD(TAG, LEDEBUG, "Could not open codec.");
        exit(EXIT_FAILURE);
    }

    //7. 解码开始准备工作
    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();

    //根据需要解码的类型，获取需要的buffer，不要忘记free
    outBuffer = (uint8_t *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecContext->width,
                                     pCodecContext->height, 1) * sizeof(uint8_t));
    //根据指定的图像参数和提供的数组设置数据指针和行数 ，数据填充到对应的pFrameYUV里面
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, outBuffer, AV_PIX_FMT_RGBA,
                         pCodecContext->width, pCodecContext->height, 1);

    pPacket = av_packet_alloc();
    leLogD(TAG, LEDEBUG, "--------------- File Information ----------------");
    av_dump_format(pFormatContext, 0, filename, 0);
    leLogD(TAG, LEDEBUG, "-------------------------------------------------");

    leLogD(TAG, LEDEBUG, "width:%d  height:%d  pix_fmt:%d", pCodecContext->width,
           pCodecContext->height, pCodecContext->pix_fmt);
    pSwsContext = sws_getContext(pCodecContext->width, pCodecContext->height,
                                 pCodecContext->pix_fmt,
                                 pCodecContext->width, pCodecContext->height,
                                 AV_PIX_FMT_RGBA,
                                 NULL, NULL, NULL, NULL);
    //默认为RGB565
//    if (ANativeWindow_setBuffersGeometry(nativeWindow, pCodecContext->width, pCodecContext->height,
//                                         WINDOW_FORMAT_RGBA_8888) < 0) {
//        leLogD(TAG, LEDEBUG, "Could not set buffers geometry");
//        ANativeWindow_release(nativeWindow);
//        exit(EXIT_FAILURE);
//    }
    int count = 0;
    while (av_read_frame(pFormatContext, pPacket) == 0) {
        if (pPacket->stream_index == videoIndex) {
            //解码数据
            if (avcodec_send_packet(pCodecContext, pPacket) != 0) {
                leLogD(TAG, LEDEBUG, "Decode end or Error.\n");
                break;
            } else {
                //处理解码数据并写入文件
                avcodec_receive_frame(pCodecContext, pFrame);

                if (sws_scale(pSwsContext, (const uint8_t *const *) pFrame->data, pFrame->linesize,
                              0, pCodecContext->height, pFrameYUV->data, pFrameYUV->linesize) ==0) {
                    continue;
                }
                count++;
                //逐行复制
//                // yuv-> 4:1:1
                int y_size = pCodecContext->width * pCodecContext->height;
                fwrite(pFrameYUV->data[0], 1, static_cast<size_t>(y_size), yuvFilePath);
                fwrite(pFrameYUV->data[1], 1, static_cast<size_t>(y_size / 4), yuvFilePath);
                fwrite(pFrameYUV->data[2], 1, static_cast<size_t>(y_size / 4), yuvFilePath);
                leLogD(TAG, LEDEBUG, "Flush Decoder: Succeed to decode frame!", count);
            }
        }
    }
    return 0;
}