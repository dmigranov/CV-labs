#include "morphology.h"
#include "lab.h"


Mat invertion(Mat orig)
{
	Mat res(orig.rows, orig.cols, orig.type());
	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
			res.at<uchar>(i, j) = 255 - orig.at<uchar>(i, j);
	return res;
}

Mat dilation(Mat orig, Mat oper)
{

	Mat res(orig.rows, orig.cols, orig.type());
	res = 0;
	//std::cout << orig;
	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			if (orig.at<uchar>(i, j) == 255) //255 - это в матрице 8U, т.е uchar; в double надо 1
				for (int x = -oper.rows / 2; x < oper.rows / 2 + 1; x++)
					for (int y = -oper.cols / 2; y < oper.cols / 2 + 1; y++)
					{
						if (i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols)
						{
							if(res.at<uchar>(i + x, j + y) == 0)
								res.at<uchar>(i + x, j + y) = oper.at<uchar>(oper.rows / 2 + x, oper.cols / 2 + y);
						}
					}
		}

	return res;
}



Mat erosion(Mat orig, Mat oper)
{
	Mat res(orig.rows, orig.cols, orig.type());
	res = 0;

	uint operNumber = 0;
	for (int x = 0; x < oper.rows; x++)
		for (int y = 0; y < oper.cols; y++)
			if (oper.at<uchar>(x, y) == 255)
				operNumber++;

	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			if (orig.at<uchar>(i, j) == 255)
			{
				uint count = 0;
				for (int x = -oper.rows / 2; x < oper.rows / 2 + 1; x++)
					for (int y = -oper.cols / 2; y < oper.cols / 2 + 1; y++)
					{
						if ((i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols) && orig.at<uchar>(i + x, j + y) == 255 && oper.at<uchar>(oper.rows / 2 + x, oper.cols / 2 + y) == 255)
						{
							count++;
						}
					}
				
				//std::cout << count << std::endl;
				if (count == operNumber)
				{

					res.at<uchar>(i, j) = 255;
				}
			}
		}

	return res;
}