#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include <android/log.h>
//#define TAG_BASE "hpplay-base"

/*
int lelogLevel = LOGGER_DEBUG;
int getlogLevel()
{
    return lelogLevel;
}

int setLogLevel(int level)
{
    lelogLevel = level;
    return 0;
}*/

#ifdef  TARGET_OS_WINDOWS

void LOG_FATAL(char *tag, char *info)
{
    printf("%s:%s", tag, info)
}

void LOG_ERROR(char *tag, char *info)
{
    printf("%s:%s", tag, info)
}

void LOG_INFO(char *tag, char *info)
{
    printf("%s:%s", tag, info);
}

void LOG_INFO(char *tag, char *info)
{
    printf("%s:%s", tag, info);
}

void LOG_DEBUG(char *tag, char *info)
{
    printf("%s:%s", tag, info);
}

#elif TARGET_OS_POSIX

void LOG_FATAL(char *tag, char *info)
{
    printf("%s:%s", tag, info)
}

void LOG_ERROR(char *tag, char *info)
{
    printf("%s:%s", tag, info)
}

void LOG_INFO(char *tag, char *info)
{
    printf("%s:%s", tag, info);
}

void LOG_INFO(char *tag, char *info)
{
    printf("%s:%s", tag, info);
}

void LOG_DEBUG(char *tag, char *info)
{
    printf("%s:%s", tag, info);
}

#elif TARGET_OS_ANDROID
#include <android/log.h>

//void LOG_FATAL(char *tag, char *info)
//{
//    __android_log_print(ANDROID_LOG_DEBUG, tag, info);
//}
//
//void LOG_ERROR(char *tag, char *info)
//{
//    __android_log_print(ANDROID_LOG_DEBUG, tag, info);
//}
//
//void LOG_WARN(char *tag, char *info)
//{
//    __android_log_print(ANDROID_LOG_DEBUG, tag, info);
//}
//void LOG_INFO(char *tag, char *info)
//{
//    __android_log_print(ANDROID_LOG_DEBUG, tag, info);
//}
//void LOG_DEBUG(char *tag, char *info)
//{
//    __android_log_print(ANDROID_LOG_DEBUG, tag, info);
//}

#else

void LOG_FATAL(char *tag, char *info)
{
    printf("%s:%s", tag, info)
}

void LOG_ERROR(char *tag, char *info)
{
    printf("%s:%s", tag, info)
}

void LOG_INFO(char *tag, char *info)
{
    printf("%s:%s", tag, info);
}

void LOG_INFO(char *tag, char *info)
{
    printf("%s:%s", tag, info);
}

void LOG_DEBUG(char *tag, char *info)
{
    printf("%s:%s", tag, info);
}

#endif

static char *logger_utf8_to_local(const char *str)
{
    char *ret = NULL;

    /* FIXME: This is only implemented on Windows for now */
#if defined(_WIN32) || defined(_WIN64)
    int wclen, mblen;
    WCHAR *wcstr;
    BOOL failed;

    wclen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    wcstr = (WCHAR *)malloc(sizeof(WCHAR) * wclen);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wcstr, wclen);

    mblen = WideCharToMultiByte(CP_ACP, 0, wcstr, wclen, NULL, 0, NULL, &failed);
    if (failed)
    {
        /* Invalid characters in input, conversion failed */
        free(wcstr);
        return NULL;
    }

    ret = (char *)malloc(sizeof(CHAR) * mblen);
    WideCharToMultiByte(CP_ACP, 0, wcstr, wclen, ret, mblen, NULL, NULL);
    free(wcstr);
#endif
    return ret;
}

/*
void logger_log(int level, bool bLoggerFlag, const char *fmt, ...)
{
    if( (false == bLoggerFlag) )
    {
        return;
    }

    switch(level)
    {
    case LOGGER_FATAL:
        LOG_FATAL(TAG_BASE,fmt);
        break;
    case LOGGER_ERROR:
        LOG_ERROR(TAG_BASE,fmt);
        break;
    case LOGGER_WARNING:
        LOG_WARN(TAG_BASE,fmt);
        break;
    case LOGGER_INFO:
        LOG_INFO(TAG_BASE,fmt);
        break;
    case LOGGER_DEBUG:
        LOG_DEBUG(TAG_BASE,fmt);
        break;
    default:
        break;
    }
}
*/
