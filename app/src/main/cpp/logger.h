#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdio.h>
#include <android/log.h>
#include <stdlib.h>
#include <libavutil/log.h>

#define TAG_BASE "hpplay-base"
#define ENABLE_DEBUG
#define TARGET_OS_ANDROID 1

//#define LOGGER_FATAL       1       /* system is unusable */
//#define LOGGER_ERROR       2       /* error conditions */
//#define LOGGER_WARNING     3       /* warning conditions */
//#define LOGGER_INFO        4       /* informational */
//#define LOGGER_DEBUG       5       /* debug-level messages */

//int getlogLevel();
//int setLogLevel(int level);

#define leLogF(module_tag, bLoggerFlag, fmt, ...)                                                      \
    if( (NULL!= module_tag) && (true == bLoggerFlag))                      \
    {                                                                                                        \
        __android_log_print(ANDROID_LOG_FATAL,TAG_BASE,"[%s:%u] "fmt, module_tag, __LINE__, ##__VA_ARGS__);       \
    }                                                                                                        \

#define leLogE(module_tag, bLoggerFlag, fmt, ...)                                                      \
    if( (NULL!= module_tag) && (true == bLoggerFlag))                      \
    {                                                                            \
        __android_log_print(ANDROID_LOG_ERROR,TAG_BASE,"[%s:%u] "fmt, module_tag,  __LINE__, ##__VA_ARGS__);      \
    }                                                                                                        \

#define leLogW(module_tag, bLoggerFlag, fmt, ...)                                                       \
    if( (NULL!= module_tag) && (true == bLoggerFlag))                    \
    {                                                                                                        \
        __android_log_print(ANDROID_LOG_WARN,TAG_BASE,"[%s:%u] "fmt, module_tag,  __LINE__, ##__VA_ARGS__);   \
    }                                                                                                        \

#define leLogI(module_tag, bLoggerFlag, fmt, ...)                                                       \
    if( (NULL!= module_tag) && (true == bLoggerFlag))                       \
    {                                                                                                        \
        __android_log_print(ANDROID_LOG_INFO,TAG_BASE,"[%s:%u] "fmt, module_tag,  __LINE__, ##__VA_ARGS__);       \
    }                                                                                                        \

#define leLogD(module_tag, bLoggerFlag, fmt, ...)                                                      \
    if( (NULL!= module_tag) && (true == bLoggerFlag))                      \
    {                                                                                                        \
        __android_log_print(ANDROID_LOG_DEBUG,TAG_BASE,"[%s:%u] "fmt, module_tag,  __LINE__, ##__VA_ARGS__);      \
    }                                                                                                        \

#define leLogN(module_tag, bLoggerFlag, fmt, ...)                                                      \
    if( (NULL!= module_tag) && (true == bLoggerFlag))                      \
    {                                                                                                        \
        __android_log_print(ANDROID_LOG_INFO,TAG_BASE, fmt, ##__VA_ARGS__);      \
    }                                                                                                        \

#define FF_LOG_TAG     "FFmpeg"
#define FF_LOG_UNKNOWN     ANDROID_LOG_UNKNOWN
#define FF_LOG_DEFAULT        ANDROID_LOG_DEFAULT
#define FF_LOG_VERBOSE     ANDROID_LOG_VERBOSE
#define FF_LOG_DEBUG         ANDROID_LOG_DEBUG
#define FF_LOG_INFO              ANDROID_LOG_INFO
#define FF_LOG_WARN           ANDROID_LOG_WARN
#define FF_LOG_ERROR        ANDROID_LOG_ERROR
#define FF_LOG_FATAL           ANDROID_LOG_FATAL
#define FF_LOG_SILENT         ANDROID_LOG_SILENT


#define VLOG(level, TAG, ...)    ((void)__android_log_vprint(level, TAG, __VA_ARGS__))
#define VLOGV(...)  VLOG(FF_LOG_VERBOSE,   FF_LOG_TAG, __VA_ARGS__)
#define VLOGD(...)  VLOG(FF_LOG_DEBUG,     FF_LOG_TAG, __VA_ARGS__)
#define VLOGI(...)  VLOG(FF_LOG_INFO,      FF_LOG_TAG, __VA_ARGS__)
#define VLOGW(...)  VLOG(FF_LOG_WARN,      FF_LOG_TAG, __VA_ARGS__)
#define VLOGE(...)  VLOG(FF_LOG_ERROR,     FF_LOG_TAG, __VA_ARGS__)


#define ALOG(level, TAG, ...)    ((void)__android_log_print(level, TAG, __VA_ARGS__))
#define ALOGV(...)  ALOG(FF_LOG_VERBOSE,   FF_LOG_TAG, __VA_ARGS__)
#define ALOGD(...)  ALOG(FF_LOG_DEBUG,     FF_LOG_TAG, __VA_ARGS__)
#define ALOGI(...)  ALOG(FF_LOG_INFO,      FF_LOG_TAG, __VA_ARGS__)
#define ALOGW(...)  ALOG(FF_LOG_WARN,      FF_LOG_TAG, __VA_ARGS__)
#define ALOGE(...)  ALOG(FF_LOG_ERROR,     FF_LOG_TAG, __VA_ARGS__)


static void ffp_log_callback_report(void *ptr, int level, const char *fmt, va_list vl) {
    int ffplv = FF_LOG_VERBOSE;
    if (level <= AV_LOG_ERROR)
        ffplv = FF_LOG_ERROR;
    else if (level <= AV_LOG_WARNING)
        ffplv = FF_LOG_WARN;
    else if (level <= AV_LOG_INFO)
        ffplv = FF_LOG_INFO;
    else if (level <= AV_LOG_VERBOSE)
        ffplv = FF_LOG_VERBOSE;
    else
        ffplv = FF_LOG_DEBUG;

    va_list vl2;
    char line[1024];
    static int print_prefix = 1;

    va_copy(vl2, vl);
    // av_log_default_callback(ptr, level, fmt, vl);
    av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
    va_end(vl2);

    ALOG(ffplv, FF_LOG_TAG, "%s", line);
}

#endif