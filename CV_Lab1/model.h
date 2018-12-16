#pragma once

#include <opencv2/opencv.hpp>

#include "lab.h"
#include "filtration.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace cv;

Mat hough(Mat orig, int threshold, double lower, double upper);
Mat hough_circle(Mat orig, int threshold, double lower, double upper);

Mat ransac(Mat orig, int threshold, double lower, double upper);


