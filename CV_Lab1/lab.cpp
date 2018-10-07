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