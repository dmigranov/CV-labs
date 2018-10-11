#include "filtration.h"




//Mat window(Mat orig)
//{}




Mat gauss_filter(Mat original, double sigma)
{

	//5x5

	Mat orig;
	original.convertTo(orig, CV_64FC3);
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
					
					if (i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols)
					{
						//for(int k = 0; k < 3; k++)
						newimg.at<Vec3d>(i, j) += orig.at<Vec3d>(i + x, j + y) * gauss.at<double>(x + 2, y + 2);
					}
				}
		}
	//std::cout << orig.at<Vec3b>(0,0) << std::endl;
	newimg.convertTo(newimg, original.type());
	//imshow("Gauss", newimg);
	//get submatrix
	return newimg;

}


void sobel_filter(Mat orig)
{
	Mat Gx = (Mat_<int>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	Mat Gy = (Mat_<int>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
	

	Mat labimg;

	cvtColor(orig, labimg, COLOR_BGR2Lab);
	std::vector<Mat> labv;
	split(labimg, labv);
	Mat L = labv[0];

	double gxv, gyv;
	Mat newimg(orig.rows, orig.cols, L.type());

	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			gxv = 0;
			gyv = 0;
			for (int x = -1; x < 2; x++)
				for (int y = -1; y < 2; y++)
				{
					//std::cout << i + x << " " << j + y << std::endl;
					if (i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols)
					{
						gxv +=
							L.at<char>(i + x, j + y) * Gx.at<int>(x + 1, y + 1);
						gyv += 
							L.at<char>(i + x, j + y) * Gy.at<int>(x + 1, y + 1);
					}
				}
			newimg.at<char>(i, j) = sqrt(gxv * gxv + gyv * gyv);
		}



	imshow("Sobel filter", newimg);

}

void canny(Mat orig)
{


}