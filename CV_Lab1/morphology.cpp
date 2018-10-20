#include "morphology.h"
#include "lab.h"



Mat dilation(Mat orig, Mat oper)
{
	Mat res(orig.rows, orig.cols, orig.type());
	res = 0;

	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			if (orig.at<uchar>(i, j) == 1)
				for (int x = -oper.rows / 2; x < oper.rows / 2 + 1; x++)
					for (int y = -oper.cols / 2; y < oper.cols / 2 + 1; y++)
					{
						if (i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols)
						{
							res.at<uchar>(i + x, j + y) = oper.at<uchar>(oper.rows / 2  + x, oper.cols / 2 + y);
						}
					}
		}

	return res;
}



Mat erosion(Mat orig, Mat oper)
{
	Mat res(orig.rows, orig.cols, orig.type());
	res = 0;



}