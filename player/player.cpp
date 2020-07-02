#ifdef __cplusplus
extern "C"
{

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#endif

}

#else

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#endif

#endif
#include <iostream>

int main()
{
    AVFormatContext* pFormatCtx;//封装格式上下文结构体，也是统领全局的结构体，保存了视频文件封装格式相关信息。
    AVCodecContext* pCodecCtx;//编码器上下文结构体，保存了视频（音频）编解码相关信息。
    AVCodec* pCodec;//每种视频（音频）编解码器(例如H.264解码器)对应一个该结构体。
    AVFrame* pFrame, * pFrameYUV;//存储一帧解码后像素（采样）数据
    uint8_t* out_buffer;//AVFrame 结构体内保存的解码数据
    AVPacket* packet;
    char filepath[] = "E:\\ffmpeg\\player\\player\\video\\demo.mp4";
    int				i, videoindex;

    av_register_all();//初始化所有组件

    pFormatCtx = avformat_alloc_context();

    //打开视频流域
    if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {
        printf("Couldn't open input stream.\n");
        return -1;
    }

    //获取视频流信息，解码器类型、宽高等
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("Couldn't find stream information.\n");
        return -1;
    }

    //查找视频流ID
    videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoindex = i;
            break;
        }
    if (videoindex == -1) {
        printf("Didn't find a video stream.\n");
        return -1;
    }

    //获取编码器上下文
    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    //查找编解码器
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        printf("Codec not found.\n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec.\n");
        return -1;
    }

    //这个函数只是分配AVFrame结构体，但data指向的内存并没有分配
    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    //计算这个格式的图片，需要多少字节来存储
    //申请空间来存放图片数据。包含源数据和目标数据  
    out_buffer = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
    //这里把av_malloc得到的内存和AVFrame关联起来
    avpicture_fill((AVPicture*)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

    packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    //Output Info-----------------------------
    printf("--------------- File Information ----------------\n");
    av_dump_format(pFormatCtx, 0, filepath, 0);
    printf("-------------------------------------------------\n");


    printf("all success\n");

}