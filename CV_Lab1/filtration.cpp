#include "filtration.h"




//Mat window(Mat orig)
//{}




void gauss_filter(Mat orig, double sigma)
{

	//5x5

	Mat gauss(5, 5, CV_64FC1);
	Mat newimg(orig.rows, orig.cols, orig.type());
	newimg = 0;
	double div = 0;
	for (int i = -2; i < 3; i++)
		for (int j = -2; j < 3; j++)
		{
			gauss.at<double>(i + 2, j + 2) = exp(-(i * i + j * j) / (2.0 * sigma * sigma)) / (2.0 * M_PI * sigma * sigma); //not simmetric! 5
			div += gauss.at<double>(i + 2, j + 2);
		}
	gauss /= div;
	//std::cout << gauss << std::endl << div << std::endl;
	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			for (int x = -2; x < 3; x++)
				for (int y = -2; y < 3; y++)
				{
					//std::cout << i + x << " " << j + y << std::endl;
					if (i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols)
					{
						//for(int k = 0; k < 3; k++)
						newimg.at<Vec3b>(i + x, j + y) += orig.at<Vec3b>(i + x, j + y) * gauss.at<double>(x + 2, y + 2);
					}
				}
		}
	//std::cout << orig.at<Vec3b>(0,0) << std::endl;

	imshow("Gauss", newimg);
	//get submatrix

}