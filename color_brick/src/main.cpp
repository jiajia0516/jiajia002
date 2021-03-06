#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include "serialport.h"

using namespace cv;
using namespace std;

#define h_w_ratio_min  2.800
#define h_w_ratio_max  3.800
#define min_area  1000


Mat frame;      //原图
Mat hsv_img;    //HSV图
Mat bin_img;    //二值图
Mat dst_img;    //输出图

int main()
{
    SerialPort port;
    if(serialisopen == 1)
    {
        port.serialSet(1);
        //initPort();//串口初始化函数
    }

    VideoCapture capture(0);
    while(1)
    {
        capture >> frame;

        cvtColor(frame,hsv_img,COLOR_BGR2HSV);
        inRange(hsv_img,Scalar(0,80,80),Scalar(15,255,255),bin_img);  //红色
//        inRange(hsv_img,Scalar(),Scalar(),bin_img);//蓝色
        medianBlur(bin_img,bin_img,7);

        line(frame, Point(320,0),Point(320,480),Scalar(0,255,255),4,8,0);     //纵向中线

        vector<vector<Point> >contours;
        vector<Vec4i>hierarchy;
        findContours(bin_img,contours,hierarchy,RETR_TREE,CHAIN_APPROX_SIMPLE,Point(0,0));

        vector<RotatedRect>minRects(contours.size());

        for (uint i = 0; i < contours.size(); ++i)
        {
            minRects[i] = minAreaRect(Mat(contours[i]));
            {
                Point2f rectPoints[4];
                minRects[i].points(rectPoints);

                double x0 = rectPoints[0].x;
                double y0 = rectPoints[0].y;
                double x1 = rectPoints[1].x;
                double y1 = rectPoints[1].y;
                double x2 = rectPoints[2].x;
                double y2 = rectPoints[2].y;               //定义返回的坐标点

                double width  = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
                double height = sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1));    //定义长宽
                double area = width * height;                             //面积
                double w = 0,h = 0;
                if(width > height)
                {
                    h = width;
                    w = height;
                }
                else
                {
                    h = height;
                    w = width;
                }

                double h_w_ratio = fabs( h / w ) ;                     // 高宽比
                double rect_x1 = (x0 + x2)/2;                 //轮廓在X轴上的中点
                double rect_y1 = (y0 + y2)/2;                 //轮廓在Y轴上的中点
                double position = rect_x1 - 320;       //判断砖头在中线的哪个方位 position>0为右侧，position<0为左侧
                Point contour_center = Point(rect_x1, rect_y1);

                if(h_w_ratio > h_w_ratio_min && h_w_ratio< h_w_ratio_max && area > min_area )
                {
                     for (int j = 0; j < 4; j++)
                    {
                        if(position+15 >0)
                       {
                            line(frame, rectPoints[j], rectPoints[(j + 1) % 4], Scalar(0,255,0), 4, 8, 0);//--
                            circle(frame, contour_center,15,Scalar( 0,255, 0),2,8,0);
                            cout<<contour_center<<endl<<endl;
                       }
                    }
                }
            }
        }
        imshow("笑果",frame);
        char (key)=(char) waitKey(30);
        if (key==27)break;
    }
   return 0;
}
