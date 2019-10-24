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
//��֡��ʾ
#define JUMP_RATE 10
//���������Ϣ
#define DEBUG_PRINT(FORMAT,VALUE) printf("file:%s lines:%d,"#VALUE" is "FORMAT"\n",__FILE__,__LINE__,VALUE)

int main(void)
{
    int ret = -1;
    cv::Mat img;
    int count = 0;
    cv::Mat* pCvMat = new cv::Mat(); //�����������
    video_t* handel = video_init("d2.avi",&ret); //�����ʼ��
    assert(handel != NULL);
    assert(ret == 0);
	/*����ʱ�����Ƶ��λ��֧����Ƶ�ļ����ͣ���ý�巽ʽ��֧��*/
    int alltime = video_get_alltime(handel); //��ȡ����ʱ��
    DEBUG_PRINT("%d",alltime);
    int state = video_seek_frame(handel,0); //��Ƶ��λ
    DEBUG_PRINT("%d",state);
    while(1)
    {
        if(count % JUMP_RATE == 0)
        {
            int num = video_get_frame(handel,pCvMat); //��ȡ��������
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
	//�ͷ���Դ
    pCvMat->release();
    video_uninit(handel);
    return 0;
}
