#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

const Mat square3x3 = (Mat_<uchar>(3, 3) << 255, 255, 255, 255, 255, 255, 255, 255, 255);
const Mat cross3x3 = (Mat_<uchar>(3, 3) << 0, 255, 0, 255, 255, 255, 0, 255, 0);
const Mat circle3x3 = cross3x3;
const Mat circle5x5 = (Mat_<uchar>(5, 5) << 0, 255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 255, 255, 0);
const Mat circle7x7 = (Mat_<uchar>(7, 7) << 0, 0, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 255, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 0, 0);
const Mat square5x5 = (Mat_<uchar>(5, 5) << 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255);
const Mat square7x7 = (Mat_<uchar>(7, 7) << 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255);


//mathematical morphology operators
//original should be binary: 255 or 0 and CV_8UC1
//NB: 255 - белый, 0 - чёрный! Рисовать белым по чёрному

Mat dilation(Mat original, Mat oper);
Mat erosion(Mat orig, Mat oper);
Mat closing(Mat orig, Mat oper); //убирает внутренние дырки. Понятно, что если два объекта близки, он считает их за один с дыркой
Mat opening(Mat orig, Mat oper);
Mat invertion(Mat orig);
uint countObjects(Mat orig);
void fillWithMark(Mat &orig, Mat &labels, uint label, uint x, uint y);