#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include "twoPass.h"
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

cv::Scalar getRandomColor()
{
	uchar r = 255 * (rand() / (1.0 + RAND_MAX));
	uchar g = 255 * (rand() / (1.0 + RAND_MAX));
	uchar b = 255 * (rand() / (1.0 + RAND_MAX));
	return cv::Scalar(b, g, r);
}

cv::Mat showColorLabel(cv::Mat label)
{
	int imgW = label.cols;
	int imgH = label.rows;
	std::map<int, cv::Scalar> colors;

	cv::Mat colorLabel = cv::Mat::zeros(imgH, imgW, CV_8UC3);
	int *pLabel = (int*)label.data;
	uchar *pColorLabel = colorLabel.data;
	for (int i = 0; i < imgH; i++)
	{
		//printf("\n");
		for (int j = 0; j < imgW; j++)
		{
			int idx = (i*imgW + j) * 3;
			int pixelValue = pLabel[i*imgW + j];
			//printf(" %02x",pixelValue);
			if (pixelValue > 0)
			{
				if (colors.count(pixelValue) <= 0)
				{
					colors[pixelValue] = getRandomColor();		
				}
					cv::Scalar color = colors[pixelValue];
					pColorLabel[idx + 0] = color[0];
					pColorLabel[idx + 1] = color[1];
					pColorLabel[idx + 2] = color[2];
			}
		}
	}

	return colorLabel;
}

int main(int argc, char *argv[])
{
	string imageName = argv[1];
	cv::Mat srcImage  = cv::imread(imageName, 1);
	if (!srcImage.data )
	{
		cout << "No image data" << endl;
		return 0;
	}
	cv::Mat image;
	cv::cvtColor(srcImage, image, CV_RGB2GRAY);

	cv::Mat threshImg;
	cv::threshold(image, threshImg, 140, 255, cv::THRESH_BINARY);
	cv::bitwise_not(threshImg, threshImg);

	cv::imshow("threshImg", threshImg);

	Mat element = getStructuringElement(MORPH_RECT,Size(3,3));
  Mat destImage;
  erode(threshImg,destImage,element);
	cv::imshow("destImage", destImage);

	element = getStructuringElement(MORPH_RECT,Size(5,5));
  dilate( destImage, destImage, element );

	cv::Mat labelImg=cv::Mat::zeros(image.size(), CV_32SC1);
	int maxLabel = -1;
	TwoPass twoPassMethod(destImage.cols*destImage.rows);
	twoPassMethod.run(destImage, labelImg, maxLabel);

	cv::imshow("labelImg", labelImg * 60);
	cv::Mat colorLabelImg = showColorLabel(labelImg);

	cv::imshow("colorLabel", colorLabelImg);

	vector< vector<cv::Point> > contours;

	findContours(destImage,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	drawContours(destImage,contours,-1,cv::Scalar(128),1); 
	imshow("dilate",destImage);
	
	cout<<contours.size()<<endl;
	double min_area = 10000.0;
	double area = 0.0;
	for( size_t i = 0; i< contours.size(); i++ ) // iterate through each contour.    
	{        
		area = contourArea( contours[i] );  //  Find the area of contour    
		//cout<<"area: " << area<<endl;     
		if( area < min_area ){min_area = area;}    
	}   
	//printf("min:%f\n",min_area);
	char ss[10] = {0};
	char * snum = ss;
	int n = 0;
	for( size_t i = 0; i< contours.size(); i++ ) // iterate through each contour.    
	{        
		area = contourArea( contours[i] );  //  Find the area of contour 
		int k = (int)(area/min_area);
		n = n + k;       
		if( k > 1 )       
		{            
			Rect r0=boundingRect(contours[i]);
			rectangle(srcImage,r0,cv::Scalar(0, 0, 255),1);
			sprintf(snum,"%d",k);
			putText(srcImage, snum, Point(r0.x, r0.y), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255), 1, 1);                 
		}    
	} 
	char str[10] = {0};
	char * num = str;
	sprintf(num,"%d",n);
	putText(srcImage, num, Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0), 2, 2);
	cv::imshow("result", srcImage);
	cv::waitKey(0);
}
