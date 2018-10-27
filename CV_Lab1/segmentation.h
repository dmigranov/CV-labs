#pragma once
#include <opencv2/opencv.hpp>
#include "lab.h"

using namespace cv;

Mat splitmerge(Mat orig);
Mat normalizedCut(Mat orig);

double homogeneity(Mat region);
double mean(Mat region);