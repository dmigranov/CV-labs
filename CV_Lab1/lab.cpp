#include "lab.h"

Mat xyzmatrix = (Mat_<double>(3, 3) << 2.768892, 1.751748, 1.13016, 1.0, 4.5907, 0.0601, 0.0, 0.056508, 5.594292);

Vec3d getXYZ(double red, double green, double blue)
{
	Vec3d rgb(red, green, blue);
	rgb /= 255;
	Mat xyz = xyzmatrix * Mat(rgb);
	xyz /= 5.6508;
	Vec3d xyzv{ xyz.at<double>(0, 0), xyz.at<double>(1, 0), xyz.at<double>(2, 0) };
	return xyzv;
}

double labfunction(double x)
{
	if (x > pow(6.0 / 29, 3))
	{
		return pow(x, 1.0 / 3);
	}
	else
	{
		return pow(29.0 / 6, 2) / 3 + 4.0 / 29;
	}
}

double getL(double y)
{
	return (116.0 * labfunction(y / Yn) - 16);
}

Mat getLMatrix(Mat BGR) //MAt at(row, col)
{
	Mat normBGR(BGR.rows, BGR.cols, CV_64FC3);
	BGR.convertTo(normBGR, CV_64FC3);
	normBGR /= 255;
	
	Mat L(BGR.rows, BGR.cols, CV_64FC1);
	
	for (int i = 0; i < BGR.rows; i++)
	{
		Vec3d * row = normBGR.ptr<Vec3d>(i);
		
		for (int j = 0; j < BGR.cols; j++)
		{
			double y = (row[j][2] + row[j][1] * 4.5907 + row[j][0] * 0.0601) / 5.6508;;
			L.at<double>(i, j) = getL(y) / 100.0;
		}
	}
	return L;		
}