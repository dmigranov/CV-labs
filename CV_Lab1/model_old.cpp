#include <random>
#include <vector>

#include "filtration.h"
using namespace cv;

Mat ransac_(Mat orig, int threshold, double lower, double upper)
{
	int maxIter = 300;

	int dZero = 10;

	//for lines
	Mat ret;
	orig.copyTo(ret);

	Mat lines = canny(orig, lower, upper);
	int rows = orig.rows;
	int cols = orig.cols;

	double centerX = cols / 2;
	double centerY = rows / 2;

	int n = 2; //для прямой

	for (int iter = 0; iter < maxIter; iter++)
	{
		std::vector<Point> points;
		int cur_n = 0;
		while (cur_n < n)
		{
			//рандомно выбираем точки
			int rx = rand() % cols;
			int ry = rand() % rows;
			if (lines.at<double>(ry, rx) == 1)
			{
				points.push_back(Point(rx, ry));
				//std::cout << rx << " " << ry << std::endl;
				cur_n++;
			}
		}
		Point p1 = points[0];
		Point p2 = points[1];
		//ax + by + c = 0
		int a = p1.y - p2.y;
		int b = p2.x - p1.x;
		int c = (p1.x - centerX) * (p2.y - centerY) - (p2.x - centerX) * (p1.y - centerY);
		//мож вычислять радиус и theta?
		std::cout << a << " " << b << " " << c << std::endl;

		/*std::cout << a * p1.x + b * p1.y + c << std::endl;
		std::cout << a * p2.x + b * p2.y + c << std::endl;*/
		//идея: точка близко к прямой, если при подстановке значение близкок нулю...
		/*std::cout << "-----------------------------" << std::endl;
		std::cout << a * (p1.x) + b * (p1.y) + c << std::endl;
		std::cout << a * (p1.x) + b * (p1.y + 1) + c << std::endl;
		std::cout << a * (p1.x) + b * (p1.y - 1) + c << std::endl;
		std::cout << a * (p1.x + 1) + b * (p1.y) + c << std::endl;
		std::cout << a * (p1.x + 1) + b * (p1.y + 1) + c << std::endl;
		std::cout << a * (p1.x + 1) + b * (p1.y - 1) + c << std::endl;
		std::cout << a * (p1.x - 1) + b * (p1.y) + c << std::endl;
		std::cout << a * (p1.x - 2) + b * (p1.y + 1) + c << std::endl;
		std::cout << a * (p1.x - 3) + b * (p1.y - 1) + c << std::endl;
		std::cout << "-----------------------------" << std::endl;*/
		int goodPointsCount = 0;
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				if (lines.at<double>(y, x) == 1 && (a * (x - centerX) + b * (y - centerY) + c) < dZero)
					goodPointsCount++;
			}
		}
		//std::cout << goodPointsCount << std::endl;
		if (goodPointsCount > threshold)
		{
			line(ret, p1, p2, Scalar(0, 0, 255), 1, LINE_4);
		}

	}
	return ret;
}