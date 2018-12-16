#include <random>
#include <vector>

#include "model.h"
#include "filtration.h"

Mat hough(Mat orig, int threshold, double lower, double upper)
{
	Mat lines;
	Mat ret;
	orig.copyTo(ret);

	lines = canny(orig, lower, upper); //получили утончЄенные линии
	/*lines = gauss_filter(orig, 5);
	lines = canny(lines, lower, upper);*/

	int rows = orig.rows;
	int cols = orig.cols;

	double centerX = cols / 2;
	double centerY = rows / 2;

	//заполн€ем аккумул€тор
	int accu_w = 180;
	double accu_h = sqrt(2) * (rows > cols ? rows : cols); //максимальный радиус; а не слишком ли большой? ћаксимальный же(rows^2 + cols^2)sqrt

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
	std::vector<std::pair<Point2d, Point2d>> lines_vector; //пара точек, а точка - пара даблов
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

					//line(ret, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 255), 1, LINE_4);
					lines_vector.push_back(std::pair<Point2d, Point2d>(Point2d(x1, y1), Point2d(x2, y2)));
				}
				else
				{
					double x1, x2, y1, y2;
					y1 = 0;
					x1 = ((r - accu_h / 2) - (y1 - centerY) * sin(theta / 180.0*M_PI)) / cos(theta / 180.0*M_PI) + centerX;
					y2 = rows;
					x2 = ((r - accu_h / 2) - (y2 - centerY) * sin(theta / 180.0*M_PI)) / cos(theta / 180.0*M_PI) + centerX;

					//(ret, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 255), 1, LINE_4);
					lines_vector.push_back(std::pair<Point2d, Point2d>(Point2d(x1, y1), Point2d(x2, y2)));
				}
			}
		}
	}


	return ret;
}

Mat hough_circle(Mat orig, int threshold, double lower, double upper)
{
	Mat ret;
	orig.copyTo(ret);

	//(r, x0, y0); r^2 = (x - x0)^2 + (y - y0)^2
	//r = 10..50
	//gauss?
	Mat lines;
	int max_r = 80, min_r = 40, dr = max_r - min_r;
	int accu_w = 180;
	lines = gauss_filter(orig, 10);
	lines = canny(lines, lower, upper);
	return lines;
	//lines = canny(orig, lower, upper);
	
	int rows = orig.rows;
	int cols = orig.cols;

	double centerX = cols / 2;
	double centerY = rows / 2;

	//unsigned int * accu = (unsigned int*)calloc(dr * (rows + 2*max_r) * (cols + 2*max_r), sizeof(unsigned int));
	unsigned int *** accu = (uint ***)calloc(dr, sizeof(uint **));
	for (int i = 0; i < dr; i++)
	{
		accu[i] = (uint **)calloc(rows + 2 * max_r, sizeof(uint**)); //rows недостаточно
		for (int j = 0; j < rows + 2 * max_r; j++)
		{
			accu[i][j] = (uint*)calloc(cols + 2 * max_r, sizeof(uint**));
		}
	}
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			if (lines.at<double>(y, x) == 1)
			{
				for (int r = min_r; r < max_r; r++)
				{
					for (int theta = 0; theta < 360; theta++)
					{
						double a = x - r * cos(theta / 180 * M_PI);
						double b = y - r * sin(theta / 180 * M_PI);

						//accu[(int)round((r + min_r) * rows * cols + (max_r + a) * rows + (max_r + b))]++; //тут ашмпка
						//std::cout << accu[(int)round((r + min_r) * rows * cols + (max_r + a) * rows + (max_r + b))] << std::endl;
						accu[r - min_r][max_r + (int)round(b)][max_r + (int)round(a)]++;
					}
				}
			}
		}
	}
	

	for (int r = min_r; r < max_r; r++)
	{
		for (int a = 0; a < cols + max_r; a++)
		{
			for (int b = 0; b < rows + max_r; b++)
			{
				if (accu[r - min_r][b][a] == 360)
					circle(ret, Point(b, a), r, Scalar(0, 0, 255));

			}
		}
	}
	
	free(accu);
	return ret;
}


Mat triangle(Mat orig, int threshold, double lower, double upper)
{
	return orig;
}


Mat ransac(Mat orig, int threshold, double lower, double upper)
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

	int n = 2; //дл€ пр€мой

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
		//мож вычисл€ть радиус и theta?
		std::cout << a << " " << b << " " << c << std::endl;

		/*std::cout << a * p1.x + b * p1.y + c << std::endl;
		std::cout << a * p2.x + b * p2.y + c << std::endl;*/
		//иде€: точка близко к пр€мой, если при подстановке значение близкок нулю...
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