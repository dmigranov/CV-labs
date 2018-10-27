#include "segmentation.h"

double homogeneity(Mat region)
{
	double dev = 0;
	double avg = mean(region);
	for (int i = 0; i < region.rows; i++)
		for (int j = 0; j < region.cols; j++)
			dev += pow(region.at<double>(i, j) - avg, 2);
	return dev / (region.cols * region.rows - 1);
}

double mean(Mat region)
{

	double sum = 0;
	for (int i = 0; i < region.rows; i++)
		for (int j = 0; j < region.cols; j++)
		{
			//std::cout << region.at<double>(i, j) << std::endl;
			sum += region.at<double>(i, j);
		}
	return sum/(region.cols * region.rows);
}

Mat splitmerge(Mat orig)
{
	Mat L = getLMatrix(orig);
	//const = 0.001? та, с которой сравниваем с deviation. по мне, норм

	

	return L; //
}

/*
-------------------------
|			|			|
|			|			|
|			|			|
------------|------------
|			|			|
|			|			|
|			|			|
-------------------------
*/


Mat normalizedCut(Mat orig)
{

}