#pragma once
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <map>
#include <stdio.h>
using namespace std;

class TwoPass
{
public:
	TwoPass(int size);
	~TwoPass();

private:
	int maxSize;
	int *parent;

private:
	int Find(int x, int parent[]);
	void Union(int x, int y, int parent[]);
	void twoPassConnectComponent(cv::Mat binaryImg, cv::Mat &dstImg);
	void findMaxLabel(cv::Mat labelImg, int & max);

public:
	void run(const cv::Mat img, cv::Mat &labelImg, int &max);

};
