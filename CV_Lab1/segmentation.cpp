#include "segmentation.h"

const double k = 0.0004;

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

	split(r);
	merge(r);

	return r.mat; //
}

void split(Region &region)
{
	Mat regmat = region.mat;



	int rows = regmat.rows;
	int cols = regmat.cols;

	if (homogeneity(regmat) > k)
	{
		region.addChild(Region(regmat(Rect(0, 0, cols / 2, rows / 2))));								
		region.addChild(Region(regmat(Rect(cols / 2, 0, cols - cols / 2, rows / 2))));
		region.addChild(Region(regmat(Rect(0, rows / 2, cols / 2, rows - rows / 2))));
		region.addChild(Region(regmat(Rect(cols / 2, rows / 2, cols - cols / 2, rows - rows / 2))));

		//или всё-таки тут?

		for (Region r : region.children)
		{
			split(r);
		}
	}
	else
	{
		region.mat = mean(regmat);
	}
}


void merge(Region &region)
{
	//изначально их всегда четыре
	Mat		hmerged1, hmerged2, vmerged1, vmerged2;
	bool	hb1, hb2, vb1, vb2;
	hconcat(region.children[0].mat, region.children[1].mat, hmerged1);
	hb1 = homogeneity(hmerged1) < k;
	hconcat(region.children[2].mat, region.children[3].mat, hmerged2);
	hb2 = homogeneity(hmerged2) < k;
	if (hb1)
	{
		region.children.erase(region.children.begin());
		region.children.erase(region.children.begin());
	}
	if (hb2)
	{

	}

	//верт

	for (uint i = 0; i < region.children.size(); i++)
	{
		
	}
}

/*
1	2
3	4
*/


//мёрджим что можем
/**/ //looks like merge should be after полной прогонки split
//даже нсли замёрджили всё равно сплитим
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