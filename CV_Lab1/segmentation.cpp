#include "segmentation.h"

//const double k = 0.0004;

const double k = 0.004;

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

	uint iterNum = 0;

	split(r, 0);

	return r.mat; 
}

void split(Region &region, uint iterNum)
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

		if (iterNum > 5)
		{
			merge(region);
		}

		for (Region r : region.children)
		{
			split(r, ++iterNum);
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


	if (region.children.size() == 0)
		return;
	hconcat(region.children[0].mat, region.children[1].mat, hmerged1);
	hb1 = homogeneity(hmerged1) < k;
	hmerged1 = (mean(region.children[0].mat) + mean(region.children[1].mat)) / 2;
	hconcat(region.children[2].mat, region.children[3].mat, hmerged2);
	hb2 = homogeneity(hmerged2) < k;
	hmerged2 = (mean(region.children[2].mat) + mean(region.children[3].mat)) / 2;
	if (hb1 && !hb2)
	{
		//std::cout << "here1" << std::endl;
		region.children.erase(region.children.begin());
		region.children.erase(region.children.begin());
		region.children.push_back(hmerged1);
	}
	else if (!hb1 && hb2)
	{
		//std::cout << "here2" << std::endl;
		region.children.erase(region.children.end() - 1);
		region.children.erase(region.children.end() - 1);
		region.children.push_back(hmerged2);
	}
	else if (hb2 && hb1)
	{
		//std::cout << "here3" << std::endl;
		region.children.erase(region.children.begin());
		region.children.erase(region.children.begin());
		region.children.erase(region.children.begin());
		region.children.erase(region.children.begin());
		region.children.push_back(hmerged1);
		region.children.push_back(hmerged2);
	}
	else //!hb1 && !hb2
	{
		vconcat(region.children[0].mat, region.children[2].mat, vmerged1);
		vb1 = homogeneity(vmerged1) < k;
		vmerged1 = (mean(region.children[0].mat) + mean(region.children[2].mat)) / 2;
		vconcat(region.children[1].mat, region.children[3].mat, vmerged2);
		vb2 = homogeneity(vmerged2) < k;
		vmerged2 = (mean(region.children[1].mat) + mean(region.children[3].mat)) / 2;

		if (vb1 && !vb2)
		{
			//std::cout << "here1" << std::endl;
			region.children.erase(region.children.begin());
			region.children.erase(region.children.begin() + 1);
			region.children.push_back(vmerged1);
		}
		else if (!hb1 && hb2)
		{
			//std::cout << "here2" << std::endl; //»—ѕ–ј¬»“№!!!!
			region.children.erase(region.children.end() - 1);
			region.children.erase(region.children.end() - 1);
			region.children.push_back(hmerged2);
		}
		else // if (hb2 && hb1)
		{
			//std::cout << "here3" << std::endl; ////»—ѕ–ј¬»“№!!!!
			region.children.erase(region.children.begin()); //not good
			region.children.erase(region.children.begin());
			region.children.erase(region.children.begin());
			region.children.erase(region.children.begin());
			region.children.push_back(hmerged1);
			region.children.push_back(hmerged2);
		}
	}

}




//»“ќ√ќ: первые п€ть шагов просто сплитим, дальше  - на каждой итерации сплитим и мЄрджим! TODO


Mat normalizedCut(Mat orig)
{
	//Mat W
	return orig;
}