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
			//
			sum += region.at<double>(i, j);
		}
	return sum/(region.cols * region.rows);
}

Mat splitmerge(Mat orig)
{

	Mat L = getLMatrix(orig);
	Region r(L);
	Mat res(r.mat.rows, r.mat.cols, r.mat.type());
	//const = 0.001? та, с которой сравниваем с deviation. по мне, норм

	/*if (homogeneity(orig) > k)
		split(L);*/
	split(r); //no merge but it definitely slits

	return r.mat; //
}

void split(Region &region_)
{
	Mat region = region_.mat;

	//const double k = 0.001;
	const double k = 0.008;
	int rows = region.rows;
	int cols = region.cols;

	if (homogeneity(region) > k)
	{
		Region r1(region(Rect(0, 0, cols / 2, rows / 2))); //11; 11/2 = 5

		split(r1);
		//imshow("r1", r1);
		Region r2(region(Rect(cols / 2, 0, cols - cols / 2, rows / 2)));
		split(r2);
		//imshow("r2", r2);
		Region r3(region(Rect(0, rows / 2, cols / 2, rows - rows / 2)));
		split(r3);
		//imshow("r3", r3);
		Region r4(region(Rect(cols / 2, rows / 2, cols - cols / 2, rows - rows / 2)));
		split(r4);
		//merge: сначала создаём r1 r2 r3 r4, потом мёрджим
		//даже нсли замёрджили всё равно сплитим
		//imshow("r4", r4);
	}
	else
	{
		region = mean(region);
	}

	//r2 = region(Rect(rows / 2, cols / 2, rows - rows / 2, cols / 2));
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
	return orig;
}