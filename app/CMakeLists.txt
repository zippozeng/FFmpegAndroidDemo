# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        ffmpeg-player

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
#        src/main/cpp/FFmpeg.cpp
        src/main/cpp/ffmpeg_player.cpp
        src/main/cpp/H264_decoder.cpp
        src/main/cpp/logger.c)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

find_path(GLES2_INCLUDE_DIR GLES2/gl2.h
        HINTS ${ANDROID_NDK})
find_library(GLES2_LIBRARY libGLESv2.so
        HINTS ${GLES2_INCLUDE_DIR}/../lib)

find_path(SL_INCLUDE_DIR SLES/OpenSLES.h
        HINTS ${ANDROID_NDK})
find_library(SL_LIBRARY libOpenSLES.so
        HINTS ${SL_INCLUDE_DIR}/../lib)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

# FFmpeg

# 加载头文件
include_directories(
        ${CMAKE_SOURCE_DIR}/src/main/cpp #h文件目录
        ${CMAKE_SOURCE_DIR}/src/main/cpp/include
        ${GLES2_INCLUDE_DIR}
        ${SL_INCLUDE_DIR})

# 加载ffmpeg
add_library(ffmpeg
        SHARED
        IMPORTED)

set_target_properties(ffmpeg
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libffmpeg.so)

message("cmake_source_dir:${CMAKE_SOURCE_DIR}")

target_link_libraries( # Specifies the target library.
        ffmpeg-player
        ffmpeg
        # Links the target library to the log library
        # included in the NDK.
        ${GLES2_LIBRARY}
        ${SL_LIBRARY}
        ${log-lib})