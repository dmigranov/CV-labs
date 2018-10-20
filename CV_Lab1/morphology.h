#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

const Mat square3x3 = (Mat_<uchar>(3, 3) << 255, 255, 255, 255, 255, 255, 255, 255, 255);
const Mat square5x5 = (Mat_<uchar>(5, 5) << 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255);

//mathematical morphology operators
//original should be binary: 255 or 0 and CV_8UC255

Mat dilation(Mat original, Mat oper);
Mat erosion(Mat orig, Mat oper);