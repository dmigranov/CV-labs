#pragma once
#include <opencv2/opencv.hpp>
#define _USE_MATH_DEFINES
#include <math.h>



using namespace cv;

void gauss_filter(Mat orig, double sigma);