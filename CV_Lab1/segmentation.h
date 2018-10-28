#pragma once
#include <opencv2/opencv.hpp>
#include "lab.h"

using namespace cv;

Mat splitmerge(Mat region);
Mat normalizedCut(Mat orig);
void split(Mat &region, Mat &res);

double homogeneity(Mat region);
double mean(Mat region);