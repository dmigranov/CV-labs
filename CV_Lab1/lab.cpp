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