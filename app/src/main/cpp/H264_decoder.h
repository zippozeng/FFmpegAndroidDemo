//
// Created by Zippo on 2019-10-12.
//

#ifndef H264_DECODER_H
#define H264_DECODER_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "logger.h"
#ifdef __cplusplus
}
#endif

class H264Decoder {

public:
    H264Decoder() {}

    ~H264Decoder() {}

public:
    // H264初始化
    int init();

    int decode(const char *outfilename, const char *filename);

//    int destory();
};


#endif //H264_DECODER_H