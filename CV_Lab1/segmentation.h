#pragma once
#include <opencv2/opencv.hpp>

#include "lab.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace cv;



class Region
{
public:
	std::vector <Region> children;
	Mat mat;
	Region(Mat m)
	{
		mat = m;
	}
	void addChild(Region child)
	{
		children.push_back(child);
	}
};

Mat splitmerge(Mat orig);
void split(Region &region, uint iterNum);
void merge(Region &region);



Mat normalizedCut(Mat orig);

Mat meanShift(Mat orig);


double homogeneity(Mat region);
double mean(Mat region);
double CIEDE2000(Vec3d Lab1, Vec3d Lab2);