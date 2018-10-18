#pragma once
#include <opencv2/opencv.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace cv;

Mat gauss_filter(Mat orig, double sigma);

Mat sobel_filter(Mat orig, Mat * grad = NULL); //right now it returns single-channel matrix. Should it be converted to three-channel one?

Mat gabor_filter(Mat orig, double theta);

Mat canny(Mat orig, double lower, double upper);

Mat otsu(Mat orig);