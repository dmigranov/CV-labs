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
	Mat addMat;
	Region(Mat m)
	{
		mat = m;
	}
	void addChild(Region child)
	{
		children.push_back(child);
	}
};

class Point5D
{
public:
	double x;
	double y;
	double b;
	double g;
	double r;

	Point5D(int x, int y, uchar b, uchar g, uchar r)
	{
		this->x = x;
		this->y = y;
		this->b = b;
		this->g = g;
		this->r = r;
	}
	Point5D()
	{

	}

	void setPoint(int x, int y, uchar b, uchar g, uchar r)
	{
		this->x = x;
		this->y = y;
		this->b = b;
		this->g = g;
		this->r = r;
	}
	void setPoint(Point5D p)
	{
		this->x = p.x;
		this->y = p.y;
		this->b = p.b;
		this->g = p.g;
		this->r = p.r;
	}
	void addPoint(Point5D p)
	{
		this->x += p.x;
		this->y += p.y;
		this->b += p.b;
		this->g += p.g;
		this->r += p.r;
	}
	void setDiv(Point5D p, int count)
	{
		this->x = p.x / count;
		this->y = p.y / count;
		this->b = p.b / count;
		this->g = p.g / count;
		this->r = p.r / count;
	}

	double colorDistance(Point5D another)
	{
		//std::cout << sqrt(pow(r/255 - another.r/255, 2) + pow(g/255 - another.g/255, 2) + pow(b/255 - another.b/255, 2)) << std::endl;
		return sqrt(pow(r / 255 - another.r / 255, 2) + pow(g / 255 - another.g / 255, 2) + pow(b / 255 - another.b / 255, 2));
	}
	double spatialDistance(Point5D another)
	{
		return sqrt(pow(x - another.x, 2) + pow(y - another.y, 2));
	}

};

Mat splitmerge(Mat orig);
void split(Region &region, uint iterNum);
void merge(Region &region);

void normalizedCut(Mat &orig, Mat mask, uint iter);
void fillWithMean(Mat &orig, Mat mask);
Mat meanShift(Mat orig, int spatialRadius, double colorRadius);
Mat normalizedCut(Mat orig);


Mat meanShift(Mat orig);


double homogeneity(Mat region);
double mean(Mat region);
double CIEDE2000(Vec3d Lab1, Vec3d Lab2);