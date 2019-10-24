/*************************************************************************
    > File Name: decode_video.h
    > Author:zhou gang 
    > Mail: aloneup@163.com
    >://item.jd.com/576925.html#none: Created Time: Wed 25 Apr 2018 10:28:51 PM PDT
 ************************************************************************/

#ifndef _DECODE_VIDEO_H
#define _DECODE_VIDEO_H

#include "opencv2/opencv.hpp"
extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
};

typedef struct video_info
{
	AVPacket *packet;
	AVFrame *pAvFrame;
	AVCodec         *pCodec;
	AVFormatContext *pFormatCtx;
	AVCodecContext  *pCodecCtx;
	SwsContext *img_convert_ctx;
    int videoindex;
}video_t;

#ifndef G_DECODE_VIDEO_H
#define G_DECODE_VIDEO_H extern
#endif
G_DECODE_VIDEO_H video_t* video_init(const char* video_filename,int*ret);
G_DECODE_VIDEO_H int video_get_frame(video_t* handel,cv::Mat* pCvMat);
G_DECODE_VIDEO_H int video_get_alltime(video_t* handel);
G_DECODE_VIDEO_H int video_seek_frame(video_t* handel,long time_start);
G_DECODE_VIDEO_H int video_uninit(video_t* handel);


#endif
