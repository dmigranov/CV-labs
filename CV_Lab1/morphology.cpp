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



	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			if (orig.at<uchar>(i, j) == 255)
			{
				int count = 0;
				for (int x = -oper.rows / 2; x < oper.rows / 2 + 1; x++)
					for (int y = -oper.cols / 2; y < oper.cols / 2 + 1; y++)
					{
						if ((i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols) && orig.at<uchar>(i + x, j + y) == oper.at<uchar>(oper.rows / 2 + x, oper.cols / 2 + y))
						{

							count++;
						}
						//std::cout << "i'm here" << std::endl;
						
						//res.at<uchar>(i + x, j + y) = ((oper.at<uchar>(oper.rows / 2 + x, oper.cols / 2 + y) + orig.at<uchar>(i + x, j + y)) % 255) * 255;
						
						//std::cout << (uint)oper.at<uchar>(oper.rows / 2 + x, oper.cols / 2 + y) << " " << (uint)orig.at<uchar>(i + x, j + y) << std::endl;
					}
				
				//std::cout << count << std::endl;
				if (count == oper.rows * oper.cols)
				{

					res.at<uchar>(i, j) = 255;
				}
			}
		}

	return res;
}