cmake_minimum_required(VERSION 3.4.1)

set(ffmpeg_DIR ${CMAKE_SOURCE_DIR}/../../../libs)

add_library( avcodec-57
             SHARED
             IMPORTED )
set_target_properties( avcodec-57
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_DIR}/${ANDROID_ABI}/libavcodec-57.so )

add_library( avfilter-6
             SHARED
             IMPORTED)
set_target_properties( avfilter-6
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_DIR}/${ANDROID_ABI}/libavfilter-6.so )

add_library( avformat-57
             SHARED
             IMPORTED)
set_target_properties( avformat-57
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_DIR}/${ANDROID_ABI}/libavformat-57.so )

add_library( avutil-55
             SHARED
             IMPORTED )
set_target_properties( avutil-55
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_DIR}/${ANDROID_ABI}/libavutil-55.so )

add_library( swresample-2
             SHARED
             IMPORTED )
set_target_properties( swresample-2
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_DIR}/${ANDROID_ABI}/libswresample-2.so )

add_library( swscale-4
             SHARED
             IMPORTED)
set_target_properties( swscale-4
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_DIR}/${ANDROID_ABI}/libswscale-4.so )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

include_directories( ffmpeg-3.3.8/include )

add_library( player
             SHARED
             src/Java_com_center_player_MainActivity.cpp
             src/Common.cpp )

find_library( log-lib
              log )

target_link_libraries( player
                       avcodec-57
                       avfilter-6
                       avformat-57
                       avutil-55
                      -landroid
                       swresample-2
                       swscale-4
                       ${log-lib} )