#pragma once
#include <opencv2/opencv.hpp>
#define _USE_MATH_DEFINES
#include <math.h>



using namespace cv;

Mat gauss_filter(Mat orig, double sigma);

Mat sobel_filter(Mat orig); //right now it returns singe-channel matrix. Should it be converted to three-channel one?

Mat sobel_filter(Mat orig, Mat * grad); //right now it returns singe-channel matrix. Should it be converted to three-channel one?

Mat canny(Mat orig);