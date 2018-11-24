#pragma once
#include <opencv2/opencv.hpp>
#include "lab.h"

using namespace cv;

const Mat Gx = (Mat_<int>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
const Mat Gy = (Mat_<int>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);

void local_features(Mat orig);
Mat harris_detector(Mat orig);
Mat forstner_detector(Mat orig);
Mat SIFT(Mat orig, double sigma);
Mat gauss_DOG(Mat &orig, double sigma, int filter_size);
Mat gauss_DOG(Mat orig, double sigma, int filter_size, int k);

bool more(double v1, double v2);
bool less(double v1, double v2);
