#ifdef __cplusplus
extern "C"
{
#include <stdio.h>
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <sdl/SDL.h>
#endif

}

#else
#include <stdio.h>
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <sdl/SDL.h>
#endif

#endif
#include <iostream>

int main(int argc, char* argv[])
{
    AVFormatContext* pFormatCtx;//封装格式上下文结构体，也是统领全局的结构体，保存了视频文件封装格式相关信息。
    AVCodecContext* pCodecCtx;//编码器上下文结构体，保存了视频（音频）编解码相关信息。
    AVCodec* pCodec;//每种视频（音频）编解码器(例如H.264解码器)对应一个该结构体。
    AVFrame* pFrame, * pFrameYUV;//存储一帧解码后像素（采样）数据
    uint8_t* out_buffer;//AVFrame 结构体内保存的解码数据
    AVPacket* packet;
    char filepath[] = "D:\\VsCode\\player\\player\\video\\demo.mp4";
    int	i, videoindex;
    int frame_cnt;
    int ret, got_picture;
    FILE* fp;

    uint32_t pitchY;
    uint32_t pitchU;
    uint32_t pitchV;

    uint8_t* avY;
    uint8_t* avU;
    uint8_t* avV;

    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

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
    //pFrame = av_frame_alloc();
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

    fp = fopen("D:\\VsCode\\player\\player\\video\\demo.yuv", "wb");

    frame_cnt = 0;
    /*
    while (av_read_frame(pFormatCtx, packet) >= 0) 
    {
        if (packet->stream_index == videoindex) 
        {
            ret = avcodec_decode_video2(pCodecCtx, pFrameYUV, &got_picture, packet);
            if (ret < 0) {
                printf("Decode Error.\n");
                return -1;
            }

            if (got_picture)
            {
                frame_cnt++;
                pitchY = pFrameYUV->linesize[0];
                pitchU = pFrameYUV->linesize[1];
                pitchV = pFrameYUV->linesize[2];

                avY = pFrameYUV->data[0];
                avU = pFrameYUV->data[1];
                avV = pFrameYUV->data[2];

                for (i = 0; i < pFrameYUV->height; i++) {
                    fwrite(avY, pFrameYUV->width, 1, fp);
                    avY += pitchY;
                }

                for (i = 0; i < pFrameYUV->height / 2; i++) {
                    fwrite(avU, pFrameYUV->width / 2, 1, fp);
                    avU += pitchU;
                }

                for (i = 0; i < pFrameYUV->height / 2; i++) {
                    fwrite(avV, pFrameYUV->width / 2, 1, fp);
                    avV += pitchV;
                }
            }
        }
    }
    */
    printf("all success\n");

    av_frame_free(&pFrameYUV);
    //av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    fclose(fp);

    return 0;

}