/*************************************************************************
	> File Name: test.cpp
	> Author:zhou gang 
	> Mail: aloneup@163.com
	> Created Time: Thu 26 Apr 2018 01:08:47 AM PDT
 ************************************************************************/
#include "decode_video.h"
#include <iostream>
#include <assert.h>
using namespace std;
//跳帧显示
#define JUMP_RATE 10
//输出调试信息
#define DEBUG_PRINT(FORMAT,VALUE) printf("file:%s lines:%d,"#VALUE" is "FORMAT"\n",__FILE__,__LINE__,VALUE)

int main(void)
{
    int ret = -1;
    cv::Mat img;
    int count = 0;
    cv::Mat* pCvMat = new cv::Mat(); //解码输出数据
    video_t* handel = video_init("d2.avi",&ret); //解码初始化
    assert(handel != NULL);
    assert(ret == 0);
	/*解码时间和视频定位仅支持视频文件类型，流媒体方式不支持*/
    int alltime = video_get_alltime(handel); //获取解码时间
    DEBUG_PRINT("%d",alltime);
    int state = video_seek_frame(handel,0); //视频定位
    DEBUG_PRINT("%d",state);
    while(1)
    {
        if(count % JUMP_RATE == 0)
        {
            int num = video_get_frame(handel,pCvMat); //获取解码数据
            if(!(*pCvMat).empty())
            {
                (*pCvMat).copyTo(img);
                resize(img,img,cv::Size(640,480));
                imshow("Test",img);
                cvWaitKey(1);
				count = 0;
            }
            if(num < 0)
            {
                break;
            }
        }
		count++;
    }
	//释放资源
    pCvMat->release();
    video_uninit(handel);
    return 0;
}
