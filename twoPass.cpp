#include "twoPass.h"

TwoPass::TwoPass(int size)
{
	int maxSize = size;
	parent = (int*)malloc(maxSize * sizeof(int));
	memset(parent, 0, maxSize);
}

TwoPass::~TwoPass()
{
	free(parent);
}

// find the root of label x
int TwoPass::Find(int x, int parent[])
{
	//assert(x < maxSize);
	int i = x;
	while (0 != parent[i])
		i = parent[i];
  //cout << "i " << i <<endl;
	return i;
}

// set label x and label y to the same connected-region
void TwoPass::Union(int x, int y, int parent[])
{
	//assert(x < maxSize && y < maxSize);
	int i = x;
	int j = y;
	while (0 != parent[i])
		i = parent[i];
	while (0 != parent[j])
		j = parent[j];
	if (i != j)
		parent[i] = j;
}

void TwoPass::twoPassConnectComponent(cv::Mat binaryImg, cv::Mat &dst)
{
	int imgW = binaryImg.cols;
	int imgH = binaryImg.rows;
	int channel = binaryImg.channels();
	int type = binaryImg.type();
	// first pass
	int label = 1;

	//init lable image
	for (int y = 0; y < imgH; y++)
	{
		for (int x = 0; x < imgW; x++)
		{
			if (binaryImg.at<uchar>(y, x) != 0)
			{
				//left and up labels
				int left = (x - 1 < 0) ? 0 : dst.at<int>(y, x - 1);
				int up   = (y - 1 < 0) ? 0 : dst.at<int>(y - 1, x);
				//at least one label is marked
				if (left != 0 || up != 0)
				{
					//two labels are marked
					if (left != 0 && up != 0)
					{
						//adopt smaller label
						dst.at<int>(y, x) = min(left, up);
						if (left <= up)
							Union(up, left, parent);
						else if (up<left)
							Union(left, up, parent);
					}
					else
					{
						//adopt larger label
						dst.at<int>(y, x) = max(left, up);
					}
				}
				else
				{
					//non of two labels is markded, so add a new label
					dst.at<int>(y, x) = ++label;
				}
			}
		}
	}
 
	//second pass 
	for (int y = 0; y < imgH; y++)
	{
//printf("\n");
		for (int x = 0; x < imgW; x++)
		{
			if (binaryImg.at<uchar>(y, x) != 0)
				dst.at<int>(y, x) = Find(dst.at<int>(y, x), parent);
//				printf("%d ",dst.at<int>(y, x));
		}
	}
}

//find the max label value
void TwoPass::findMaxLabel(cv::Mat labelImg, int &max)
{
	int imgH = labelImg.rows;
	int imgW = labelImg.cols;
	int *pLabel = (int*)labelImg.data;

	for (int i = 0; i < imgH; i++)
	{
		for (int j = 0; j < imgW; j++)
		{
			int idx = i*imgW + j;
			if (pLabel[idx] > max) max = pLabel[idx];
		}
	}
}

void TwoPass::run(const cv::Mat threshImg, cv::Mat &labelImg, int &max)
{
	//detect connected-region using two Pass method
	twoPassConnectComponent(threshImg, labelImg);

	//find the largest label
	findMaxLabel(labelImg, max);
	//printf("max=%d\n",max);


  cout << "max " << max <<endl;
}
