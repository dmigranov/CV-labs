#pragma once
#include <opencv2/opencv.hpp>
#include "lab.h"

using namespace cv;

class Region
{
public:
	std::vector <Region> children;
	Mat mat;
	Region(Mat m)
	{
		mat = getLMatrix(m);
	}

	


};

Mat splitmerge(Mat orig);
Mat normalizedCut(Mat orig);
void split(Region &region, Mat &res);

double homogeneity(Mat region);
double mean(Mat region);