#include "model.h"

Mat hough(Mat orig)
{
	Mat lines;
	lines = canny(orig, 0.2, 0.4); //получили утончёенные линии

	int rows = orig.rows;
	int cols = orig.cols;

	double centerX = cols / 2;
	double centerY = rows / 2;

	//заполняем аккумулятор
	int accu_w = 180;
	double accu_h = sqrt(2) * (rows > cols ? rows : cols); //а не слишком ли большой радиус? Максимальный же(rows^2 + cols^2)sqrt
	//std::cout << accu_h << std::endl;
	unsigned int * accu = new unsigned int[accu_h * accu_w];
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			if (lines.at<double>(y, x) == 1)
			{
				for (int theta = 0; theta < 180; theta++)
				{
					double r = (x - centerX) * cos(theta / 180.0*M_PI) + (y - centerY) * sin(theta / 180.0*M_PI);
					//std::cout << r << std::endl;
					accu[(int)((round(r + accu_h/2) * 180)) + theta]++; //разобрать
				}
			}
		}
	}


	//search the accumulator



	return lines;
}
