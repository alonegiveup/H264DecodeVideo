/*************************************************************************
	> File Name: decode_video.cpp
	> Author:zhou gang 
	> Mail: aloneup@163.com
	> Created Time: Wed 25 Apr 2018 10:29:07 PM PDT
 ************************************************************************/
#include "decode_video.h"

G_DECODE_VIDEO_H video_t* video_init(const char* video_filename,int *ret)
{
    video_t* video_info = (video_t*)malloc(sizeof(video_t));
    video_info->packet = NULL;
    video_info->pAvFrame = NULL;
    video_info->pCodec = NULL;
    video_info->pFormatCtx = NULL;
    video_info->pCodecCtx = NULL;
    video_info->img_convert_ctx = NULL;
    video_info->videoindex = -1;
	av_register_all();
	if (avformat_open_input(&(video_info->pFormatCtx),video_filename, NULL, NULL) != 0)
	{
        //无法打开文件
        (*ret) = -1;
        return NULL;
	}
	if (avformat_find_stream_info(video_info->pFormatCtx,NULL) < 0)
	{
        //无法查找到流信息
	    (*ret) = -2;
        return NULL;
	}
    for(int i = 0;i < video_info->pFormatCtx->nb_streams;i++)
    {
        
		if (video_info->pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_info->videoindex = i;
			break;
		}
    }
    if(video_info->videoindex == -1)
    {
       //无法找到视频流
        (*ret) = -3;
        return NULL;
    }
	video_info->pCodecCtx = video_info->pFormatCtx->streams[video_info->videoindex]->codec;

	video_info->pCodec = avcodec_find_decoder(video_info->pCodecCtx->codec_id);
	if (video_info->pCodec == NULL)
	{
        (*ret) = -4;
        return NULL;
	}

	if (avcodec_open2(video_info->pCodecCtx,video_info->pCodec, NULL) < 0)
	{
        //无法打开解码器
        (*ret) = -5;
        return NULL;
	}
	video_info->pAvFrame = av_frame_alloc();
    int y_size = video_info->pCodecCtx->width * video_info->pCodecCtx->height;
	video_info->packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	av_new_packet(video_info->packet, y_size);
    (*ret) = 0;
    return video_info;

}

void video_getimg(AVCodecContext * pCodecCtx, SwsContext * img_convert_ctx, AVFrame * pFrame,cv::Mat* pCvMat)
{
	if (pCvMat->empty())
	{
		pCvMat->create(cv::Size(pCodecCtx->width, pCodecCtx->height), CV_8UC3);
	}

	AVFrame *pFrameRGB = NULL;
	uint8_t  *out_bufferRGB = NULL;
	pFrameRGB = av_frame_alloc();

	//给pFrameRGB帧加上分配的内存;
	int size = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
	out_bufferRGB = new uint8_t[size];
	avpicture_fill((AVPicture *)pFrameRGB, out_bufferRGB, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

	//YUV to RGB
	sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

	memcpy(pCvMat->data, out_bufferRGB, size);

	delete[] out_bufferRGB;
	av_free(pFrameRGB);
}

G_DECODE_VIDEO_H int video_get_frame(video_t* handel,cv::Mat* pCvMat)
{
    int result = 0;
    int pic_got = -1;
    result = av_read_frame(handel->pFormatCtx,handel->packet);
    if(result < 0)
    {
        //视频播放完成
        pCvMat = NULL;
        return -6;
    }
	//此处需注意，视频播放完成后，并不会输出-6，而是会再进行解码导致解码错误输出-7
	if (handel->packet->stream_index == handel->videoindex)
	{
		int state = avcodec_decode_video2(handel->pCodecCtx, handel->pAvFrame, &pic_got, handel->packet);
		if (state < 0)
		{
            //解码错误
            pCvMat = NULL;
            return 0;
		}
		if (pic_got)
		{
			if (handel->img_convert_ctx == NULL)
            {
				handel->img_convert_ctx = sws_getContext(handel->pCodecCtx->width, handel->pCodecCtx->height,handel->pCodecCtx->pix_fmt, handel->pCodecCtx->width, handel->pCodecCtx->height,AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
			}
			if (pCvMat->empty())
			{
				pCvMat->create(cv::Size(handel->pCodecCtx->width, handel->pCodecCtx->height), CV_8UC3);
			}

			if (handel->img_convert_ctx != NULL)
			{
				video_getimg(handel->pCodecCtx, handel->img_convert_ctx, handel->pAvFrame,pCvMat);
			}
		}
	}
	av_free_packet(handel->packet);
	return 0;
}

G_DECODE_VIDEO_H int video_get_alltime(video_t* handel)
{
	int hours, mins, secs, us;
	if (handel->pFormatCtx->duration != AV_NOPTS_VALUE)
	{
		int64_t duration = handel->pFormatCtx->duration + 5000;
		secs = duration / AV_TIME_BASE;
		us = duration % AV_TIME_BASE;
		mins = secs / 60;
		secs %= 60;
		hours = mins / 60;
		mins %= 60;
		return (hours * 3600 + mins * 60 + secs);
	}
	else
	{
		return 0;
	}
}

G_DECODE_VIDEO_H int video_seek_frame(video_t* handel,long time_start)
{
	int64_t seek_pos = 0;
	if (time_start < 0)
    { 
		return -1;
    }
	seek_pos = time_start * AV_TIME_BASE;
	if (handel->pFormatCtx->start_time != AV_NOPTS_VALUE)
    {
		seek_pos += handel->pFormatCtx->start_time;
    }
	if (av_seek_frame(handel->pFormatCtx, -1, seek_pos, AVSEEK_FLAG_ANY) < 0)
	{
		return -2;
	}
	return 0;
}

G_DECODE_VIDEO_H int video_uninit(video_t* handel)
{
	if(handel != NULL)
    {
        av_free_packet(handel->packet);
        avcodec_close(handel->pCodecCtx);
        avformat_close_input(&(handel->pFormatCtx));
        return 0;
    }
    else
    {
        return -1;
    }
}

