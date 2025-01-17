cmake_minimum_required(VERSION 3.18.1)
include_directories(../jni/include)
include_directories(../../../)
include_directories(../../../libav)



# Declares and names the project.
project("untrunc")


add_library(avcodec
        STATIC
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../jni/3d/libav/${CMAKE_ANDROID_ARCH_ABI}/libavcodec.a
        )
add_library(avfilter
        STATIC
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../jni/3d/libav/${CMAKE_ANDROID_ARCH_ABI}/libavfilter.a
        )
add_library(avformat
        STATIC
        IMPORTED)
set_target_properties(avformat
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../jni/3d/libav/${CMAKE_ANDROID_ARCH_ABI}/libavformat.a
        )
add_library(avutil
        STATIC
        IMPORTED)
set_target_properties(avutil
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../jni/3d/libav/${CMAKE_ANDROID_ARCH_ABI}/libavutil.a
        )
add_library(avresample
        STATIC
        IMPORTED)
set_target_properties(avresample
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../jni/3d/libav/${CMAKE_ANDROID_ARCH_ABI}/libavresample.a
        )
add_library(avswscale
        STATIC
        IMPORTED)
set_target_properties(avswscale
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../jni/3d/libav/${CMAKE_ANDROID_ARCH_ABI}/libavswscale.a
        )



find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )
find_library(
        android-lib
        android
)
find_library(
        z-lib
        z
)
add_library( # Sets the name of the library.
        untrunc

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ../jni/impl/com_hejunsaturday_videoinpainting_VideoUntrunc.cpp
        ../../../file.cpp
        ../../../codec_avc1.cpp
        ../../../codec_mp4a.cpp
        ../../../codec_pcm.cpp
        ../../../codec_apch.cpp
        ../../../codec.cpp
        ../../../codec_hev1.cpp
        ../../../codec_gpmd.cpp
        ../../../codecstats.cpp
        ../../../codec_text.cpp
        ../../../codec_mbex.cpp
        ../../../codec_unknown.cpp
        ../../../codec_camm.cpp
        ../../../track.cpp
        ../../../codec_mp4v.cpp
        ../../../mp4.cpp
        ../../../codec_rtp.cpp
        ../../../atom.cpp
        ../../../codec_tmcd.cpp
        ../../../codec_alac.cpp
        ../../../log.cpp
        ../../../codec_mijd.cpp
        ../../../codec_fdsc.cpp
        )
target_link_libraries( # Specifies the target library.
        untrunc
        avformat
        avcodec
        avresample
        avutil

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${z-lib}
        ${android-lib})