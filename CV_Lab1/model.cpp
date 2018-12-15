#include "model.h"

Mat hough(Mat orig, int threshold, double lower, double upper)
{
	Mat lines;
	Mat ret;
	orig.copyTo(ret);
	lines = canny(orig, lower, upper); //получили утончЄенные линии

	int rows = orig.rows;
	int cols = orig.cols;

	double centerX = cols / 2;
	double centerY = rows / 2;

	//заполн€ем аккумул€тор
	int accu_w = 180;
	double accu_h = sqrt(2) * (rows > cols ? rows : cols); //максимальный радиус; а не слишком ли большой? ћаксимальный же(rows^2 + cols^2)sqrt
	//std::cout << accu_h << std::endl;
	//unsigned int * accu = new unsigned int[accu_h * accu_w];
	unsigned int * accu = (unsigned int*)calloc((accu_h + 1) * accu_w, sizeof(unsigned int)); //?
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			if (lines.at<double>(y, x) == 1)
			{
				for (int theta = 0; theta < 180; theta++)
				{
					double r = (x - centerX) * cos(theta / 180.0*M_PI) + (y - centerY) * sin(theta / 180.0*M_PI);
					accu[(int)((round(r + accu_h/2) * accu_w)) + theta]++; //т.к массив по сути одномерный
				}
			}
		}
	}

	//search in the accumulator for maximas


	//завести вектор дл€ линий?

	for (int r = 0; r < accu_h; r++)
	{
		for (int theta = 0; theta < accu_w; theta++)
		{
			if (accu[r*accu_w + theta] > threshold)
			{
				//std::cout << accu[r*accu_w + theta] << std::endl;

				
				uint max = accu[r*accu_w + theta];
				for (int dt = -4; dt <= 4; dt++)
				{
					for (int dr = -4; dr <= 4; dr++)
					{
						if (r + dr >= 0 && theta + dt >= 0 && r + dr < accu_h && theta + dt < accu_w)
						{
							if (accu[(r + dr)*accu_w + (theta + dt)] > max)
							{
								max = accu[(r + dr)*accu_w + (theta + dt)];
								dr = dt = 5;
							}
						}
					}
				}
				if (max > accu[r*accu_w + theta])
				{
					std::cout << max << std::endl;
					continue;
				}

				if (theta >= 45 && theta <= 135)
				{
					double x1, x2, y1, y2;
					//y = r - xcos(t) / sint
					x1 = 0;
					y1 = ((r - accu_h / 2) - (x1 - centerX) * cos(theta / 180.0*M_PI)) / sin(theta / 180.0*M_PI) + centerY;
					x2 = cols;
					y2 = ((r - accu_h / 2) - (x2 - centerX) * cos(theta / 180.0*M_PI)) / sin(theta / 180.0*M_PI) + centerY;

					line(ret, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 255), 1, LINE_4);

					//std::cout << x1 << " " << x2 << " " << y1 << " " << y2 << std::endl;
				}
				else
				{
					double x1, x2, y1, y2;
					y1 = 0;
					x1 = ((r - accu_h / 2) - (y1 - centerY) * sin(theta / 180.0*M_PI)) / cos(theta / 180.0*M_PI) + centerX;
					y2 = rows;
					x2 = ((r - accu_h / 2) - (y2 - centerY) * sin(theta / 180.0*M_PI)) / cos(theta / 180.0*M_PI) + centerX;

					line(ret, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 255), 1, LINE_4);
				}
			}
		}
	}




	return ret;
}
