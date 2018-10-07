#pragma once
#include <opencv2/opencv.hpp>

#define Xn 0.9504
#define Yn 1.0
#define Zn 1.0888 //если не умножать rgb/255 на 100

using namespace cv;




Vec3d getXYZ(double red, double green, double blue);

double labfunction(double x);
double getL(double y);