#ifndef _Included_com_center_player_MainActivity
#define _Included_com_center_player_MainActivity

#include <jni.h>
#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "Common.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_com_center_player_MainActivity_nativePlay(JNIEnv *env, jclass thiz, jstring jFilePath, jobject surface)
{
    const char* filePath = env->GetStringUTFChars(jFilePath, NULL);

    av_register_all();

    AVFormatContext *pFormatCtx = avformat_alloc_context();

    // Open video file
    if (avformat_open_input(&pFormatCtx, filePath, NULL, NULL) != 0) {
        LOGE("Couldn't open file:%s\n", filePath);
        return -1; // Couldn't open file
    }

    env->ReleaseStringUTFChars(jFilePath, filePath);

    // Retrieve stream information
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Couldn't find stream information.");
        return -1;
    }

    // Find the first video stream
    //找到第一个视频流，因为里面的流还有可能是音频流或者其他的，我们摄像头只关心视频流
    int videoStream = -1;
    for (auto i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO
            && videoStream < 0) {
            videoStream = i;
            break;
        }
    }

    if (videoStream == -1) {
        LOGE("Didn't find a video stream or audio steam.");
        return -1; // Didn't find a video stream
    }
    LOGI("找到视频流");

    AVCodecParameters *pCodecPar = pFormatCtx->streams[videoStream]->codecpar;
    //查找解码器
    //获取一个合适的编码器pCodec find a decoder for the video stream
    AVCodec *pCodec = avcodec_find_decoder(pCodecPar->codec_id);
    if (pCodec == NULL) {
        LOGE("Couldn't find Codec.\n");
        return -1;
    }
    LOGI("获取解码器");

    //打开这个编码器，pCodecCtx表示编码器上下文，里面有流数据的信息
    // Get a pointer to the codec context for the video stream
    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);

    // Copy context
    if (avcodec_parameters_to_context(pCodecCtx, pCodecPar) != 0) {
        fprintf(stderr, "Couldn't copy codec context");
        return -1; // Error copying codec context
    }

    LOGI("视频流帧率：%d fps\n", pFormatCtx->streams[videoStream]->r_frame_rate.num / pFormatCtx->streams[videoStream]->r_frame_rate.den);

    int iTotalSeconds = (int) pFormatCtx->duration / 1000000;
    int iHour = iTotalSeconds / 3600;//小时
    int iMinute = iTotalSeconds % 3600 / 60;//分钟
    int iSecond = iTotalSeconds % 60;//秒
    LOGI("持续时间：%02d:%02d:%02d\n", iHour, iMinute, iSecond);
    LOGI("视频时长：%d微秒\n", pFormatCtx->streams[videoStream]->duration);
    LOGI("持续时间：%d微秒\n", (int) pFormatCtx->duration);
    LOGI("获取解码器SUCESS");
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec.");
        return -1; // Could not open codec
    }
    LOGI("获取native window");

    // 获取native window
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    LOGI("获取视频宽高");

    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;
    LOGI("设置native window的buffer大小,可自动拉伸");

    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;
    LOGI("Allocate video frame");

    // Allocate video frame
    AVFrame *pFrame = av_frame_alloc();
    LOGI("用于渲染");

    // 用于渲染
    AVFrame *pFrameRGBA = av_frame_alloc();
    if (pFrameRGBA == NULL || pFrame == NULL) {
        LOGE("Could not allocate video frame.");
        return -1;
    }
    LOGI("Determine required buffer size and allocate buffer");

    // Determine required buffer size and allocate buffer
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height, 1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height, 1);
    LOGI("由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换");

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width/*视频宽度*/,
                                                pCodecCtx->height/*视频高度*/,
                                                pCodecCtx->pix_fmt/*像素格式*/,
                                                pCodecCtx->width/*目标宽度*/,
                                                pCodecCtx->height/*目标高度*/,
                                                AV_PIX_FMT_RGBA/*目标格式*/,
                                                SWS_BICUBIC/*图像转换的一些算法*/,
                                                NULL,
                                                NULL,
                                                NULL);
    if (sws_ctx == NULL) {
        LOGE("Cannot initialize the conversion context!\n");
        return -1;
    }
    LOGI("格式转换成功");
    LOGE("开始播放");

    int ret;
    AVPacket packet;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream) {
            //该楨位置
            float timestamp = packet.pts * av_q2d(pFormatCtx->streams[videoStream]->time_base);
            LOGI("timestamp=%f", timestamp);
            // 解码
            ret = avcodec_send_packet(pCodecCtx, &packet);
            if (ret < 0) {
                break;
            }
            while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {//绘图
                // lock native window buffer
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
                // 格式转换
                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);
                // 获取stride
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = pFrameRGBA->data[0];
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                for (auto h = 0; h < videoHeight; ++h) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }
                ANativeWindow_unlockAndPost(nativeWindow);
            }
        }
        av_packet_unref(&packet);
    }

    LOGE("播放完成");
    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);
    return 0;
}

#ifdef __cplusplus
}
#endif
#endif
