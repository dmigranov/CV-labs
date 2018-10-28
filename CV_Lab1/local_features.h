#pragma once
#include <opencv2/opencv.hpp>
#include "lab.h"

using namespace cv;

Mat harris_detector(Mat orig);
Mat forstner_detector(Mat orig);
Mat SIFT(Mat orig);