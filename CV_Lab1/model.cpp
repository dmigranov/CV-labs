#include <random>
#include <vector>

#include "model.h"
#include "filtration.h"

Mat hough(Mat orig, int threshold, double lower, double upper)
{
	Mat lines;
	Mat ret;
	orig.copyTo(ret);

	//lines = canny(orig, lower, upper); //������ ��������!!!
	
	lines = gauss_filter(orig, 5);
	lines = canny(lines, lower, upper); //�������� ��������
	

	int rows = orig.rows;
	int cols = orig.cols;

	double centerX = cols / 2;
	double centerY = rows / 2;

	//��������� �����������
	int accu_w = 180;
	double accu_h = sqrt(2) * (rows > cols ? rows : cols); //������������ ������; � �� ������� �� �������? ������������ ��(rows^2 + cols^2)sqrt

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
					accu[(int)((round(r + accu_h/2) * accu_w)) + theta]++; //�.� ������ �� ���� ����������
				}
			}
		}
	}

	//search in the accumulator for maximas


	//������� ������ ��� �����?
	std::vector<std::pair<Point2d, Point2d>> lines_vector; //���� �����, � ����� - ���� ������
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

					line(ret, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 255), 1, LINE_4);
					lines_vector.push_back(std::pair<Point2d, Point2d>(Point2d(x1, y1), Point2d(x2, y2)));
				}
				else
				{
					double x1, x2, y1, y2;
					y1 = 0;
					x1 = ((r - accu_h / 2) - (y1 - centerY) * sin(theta / 180.0*M_PI)) / cos(theta / 180.0*M_PI) + centerX;
					y2 = rows;
					x2 = ((r - accu_h / 2) - (y2 - centerY) * sin(theta / 180.0*M_PI)) / cos(theta / 180.0*M_PI) + centerX;

					line(ret, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 255), 1, LINE_4);
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
	
	int rows = orig.rows;
	int cols = orig.cols;

	double centerX = cols / 2;
	double centerY = rows / 2;


	unsigned int *** accu = (uint ***)calloc(dr, sizeof(uint **));
	for (int i = 0; i < dr; i++)
	{

		accu[i] = (uint **)calloc(rows, sizeof(uint**)); //rows ������������
		for (int j = 0; j < rows; j++)
		{
			accu[i][j] = (uint*)calloc(cols, sizeof(uint*));
		}

	}


	int minr2 = min_r * min_r;
	int maxr2 = (max_r - 1) * (max_r - 1);
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			if (lines.at<double>(y, x) == 1)
			{
				for (int y0 = 0; y0 < rows; y0++)
				{
					for (int x0 = 0; x0 < cols; x0++)
					{
						double r;
						//r = sqrt(pow(x-x0,2) + pow(y-y0,2));
						r = pow(x - x0, 2) + pow(y - y0, 2);

						if (r >= minr2 && r < maxr2)
						{
							accu[(int)round(sqrt(r)) - min_r][y0][x0]++;
						}
					}
				}
			}
			//std::cout << y << " " << x << std::endl;
		}
	}
	std::cout << "HERE" << std::endl;

	for (int r = min_r; r < max_r; r++)
	{
		for (int a = 0; a < cols; a++)
		{
			for (int b = 0; b < rows; b++)
			{
				if (accu[r - min_r][b][a] >= threshold)
				{
					std::cout << b << " " << a << " " << r << std::endl;
					circle(ret, Point(a, b), r, Scalar(0, 255, 0), 2, LINE_4, 0);
				}

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
	//int maxIter = 30;
	int maxIter = 300;

	int dZero = 2.5;

	//for lines
	Mat ret;
	orig.copyTo(ret);

	Mat lines = canny(orig, lower, upper);
	int rows = orig.rows;
	int cols = orig.cols;

	double centerX = cols / 2;
	double centerY = rows / 2;


	int n = 2; //��� ������

	for (int iter = 0; iter < maxIter; iter++)
	{
		std::vector<Point> points;
		int cur_n = 0;
		while (cur_n < n)
		{
			//�������� �������� �����
			int rx = rand() % cols;
			int ry = rand() % rows;
			if (lines.at<double>(ry, rx) == 1)
			{
				points.push_back(Point(rx, ry));

				cur_n++;
			}
		}
		Point p1 = points[0];
		Point p2 = points[1];
		//ax + by + c = 0
		long int a = p2.y - p1.y;
		long int b = p1.x - p2.x;
		long int c = (p2.x) * (p1.y) - (p1.x) * (p2.y);
	
		double x1, x2, y1, y2;
		x1 = p1.x - centerX;
		x2 = p2.x - centerX;
		y1 = p1.y - centerY;
		y2 = p2.y - centerY;

		double theta = atan2(p1.x - p2.x, p2.y - p1.y);

		//atan2(p1.x - p2.x, p2.y - p1.y) - ���� �� ������ �������� ����!
		//std::cout << x1 << " " << y1 << " " << x2 << " " << y2 << " " << theta / M_PI * 180 << " " << x1 * cos(theta) + y1 * sin(theta) << " " << x2* cos(theta) + y2 * sin(theta) << std::endl;
		double r = x1 * cos(theta) + y1 * sin(theta);

		int goodPointsCount = 0;
		//int nx1 = 0, nx2 = cols, ny1 = 0, ny2 = rows;
		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				
				if (lines.at<double>(y, x) == 1 && abs((x - centerX)*cos(theta) + (y - centerY) * sin(theta) - r) < dZero)
				{
					//std::cout << abs((x - centerX)*cos(theta) + (y - centerY) * sin(theta) - r) << " " << x << " " << y << std::endl;
					/*if (x > nx1)
						nx1 = x;
					if (x < nx2)
						nx2 = x;
					if (y > ny1)
						ny1 = y;
					if (y < ny2)
						ny2 = y;*/

					goodPointsCount++;
				}
			}
		}
		//std::cout << goodPointsCount << std::endl;
		if (goodPointsCount > threshold)
		{
			line(ret, p1, p2, Scalar(0, 0, 255), 1, LINE_4);
			//line(ret, Point(0, b/(-c)), Point(a/(-c), 0), Scalar(0, 0, 255), 1, LINE_4);
		}

	}
	return ret;
}