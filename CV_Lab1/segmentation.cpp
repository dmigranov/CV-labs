#include "segmentation.h"


//const double k = 0.0004;

const double k = 0.0018;

double CIE76(Vec3d Lab1, Vec3d Lab2)
{
	double L2, L1, a2, a1, b2, b1;
	L2 = Lab2[0];
	L1 = Lab1[0];
	a2 = Lab2[1];
	a1 = Lab1[1];
	b2 = Lab2[2];
	b1 = Lab1[2];

	return sqrt(pow(L2 - L1, 2) + pow(a2 - a1, 2) + pow(b2 - b1, 2));
}


double CIEDE2000(Vec3d Lab1, Vec3d Lab2)
{
	double L2, L1, a2, a1, b2, b1;
	L2 = Lab2[0];
	L1 = Lab1[0];
	a2 = Lab2[1];
	a1 = Lab1[1];
	b2 = Lab2[2];
	b1 = Lab1[2];

	double c1 = sqrt(a1*a1 + b1 * b1);
	double c2 = sqrt(a2*a2 + b2 * b2);
	double _c = (c1 + c2) / 2; //c с крышкой
	double a1_ = a1 + a1 / 2 * (1 - sqrt(pow(_c, 7) / (pow(_c, 7) + pow(25, 7)))); //a1'
	double a2_ = a2 + a2 / 2 * (1 - sqrt(pow(_c, 7) / (pow(_c, 7) + pow(25, 7)))); //a1'
	double c1_ = sqrt(a1_ * a1_ + b1 * b1);
	double c2_ = sqrt(a2_ * a2_ + b2 * b2);
	double h1_ = atan2(b1, a1_);
	h1_ = (h1_ + M_PI) / M_PI * 180; //проверить
	double h2_ = atan2(b2, a2_);
	h2_ = (h2_ + M_PI) / M_PI * 180;
	double dh_; //delta h_

	if (abs(h1_ - h2_) <= 180)
		dh_ = h2_ - h1_;
	else if (abs(h1_ - h2_) > 180 && h2_ <= h1_)
		dh_ = h2_ - h1_ + 360;
	else
		dh_ = h2_ - h1_ - 360;


	double dH_; //H большое!
	dH_ = 2 * sqrt(c1_ * c2_) * sin(dh_ / 2);
	//_H_ //H' с крышкой


	return 0;
}

double CIEDE(Vec3d Lab1, Vec3d Lab2)
{
	return CIE76(Lab1, Lab2);
}

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
	hconcat(region.children[2].mat, region.children[3].mat, hmerged2);
	hb2 = homogeneity(hmerged2) < k;
	
	if (hb1 && !hb2)
	{
		//hmerged1 = (mean(region.children[0].mat) + mean(region.children[1].mat)) / 2;
		region.children.erase(region.children.begin());
		region.children.erase(region.children.begin());
		region.children.push_back(hmerged1);
	}
	else if (!hb1 && hb2)
	{
		//hmerged2 = (mean(region.children[2].mat) + mean(region.children[3].mat)) / 2;
		region.children.erase(region.children.end() - 1);
		region.children.erase(region.children.end() - 1);
		region.children.push_back(hmerged2);
	}
	else if (hb2 && hb1)
	{
		//std::cout << "here3" << std::endl;
		//hmerged1 = (mean(region.children[0].mat) + mean(region.children[1].mat)) / 2;
		//hmerged2 = (mean(region.children[2].mat) + mean(region.children[3].mat)) / 2;
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
		vconcat(region.children[1].mat, region.children[3].mat, vmerged2);
		vb2 = homogeneity(vmerged2) < k;
		

		if (vb1 && !vb2)
		{
			//vmerged1 = (mean(region.children[0].mat) + mean(region.children[2].mat)) / 2;
			region.children.erase(region.children.begin());
			region.children.erase(region.children.begin() + 1);
			region.children.push_back(vmerged1);
		}
		else if (!vb1 && vb2)
		{
			//vmerged2 = (mean(region.children[1].mat) + mean(region.children[3].mat)) / 2;
			region.children.erase(region.children.begin() + 1);		//ft
			region.children.erase(region.children.end() - 1);		//ft	//ftft - > fft
			region.children.push_back(vmerged2);
		}
		else if (vb2 && vb1)
		{
			//vmerged1 = (mean(region.children[0].mat) + mean(region.children[2].mat)) / 2;
			//vmerged2 = (mean(region.children[1].mat) + mean(region.children[3].mat)) / 2;
			region.children.erase(region.children.begin()); 
			region.children.erase(region.children.begin());
			region.children.erase(region.children.begin());
			region.children.erase(region.children.begin());
			region.children.push_back(vmerged1);
			region.children.push_back(vmerged2);
		}
	}

}

//ИТОГО: первые пять шагов просто сплитим, дальше  - на каждой итерации сплитим и мёрджим! TODO


Mat normalizedCut(Mat orig)
{
	Mat W(orig.rows * orig.cols, orig.rows * orig.cols, CV_64FC1);
	uint rows = orig.rows;
	Mat D(W.rows, W.cols, W.type());
	W = 0;
	D = 0;

	for (int i = 0; i < orig.rows; i++)
	{
		for (int j = 0; j < orig.cols; j++)
		{
			Vec3b bgr = orig.at<Vec3b>(i, j);
			Vec3d lab1 = getLab(bgr[2], bgr[1], bgr[0]);
			double sum = 0;
			if (i >= 1)
			{
				bgr = orig.at<Vec3b>(i - 1, j);
				Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
				double ciede = CIEDE(lab1, lab2);
				W.at<double>(i * rows + j, (i - 1) * rows + j) = ciede;
				sum += ciede;
				//может, стоит как-то сразу для (j,i) и сократить цикл?
				if (j >= 1)
				{
					bgr = orig.at<Vec3b>(i, j - 1);
					Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
					double ciede = CIEDE(lab1, lab2);
					W.at<double>(i * rows + j, i * rows + j - 1) = ciede;
					sum += ciede;
				}
				if (i < orig.rows - 1)
				{
					bgr = orig.at<Vec3b>(i + 1);
					Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
					double ciede = CIEDE(lab1, lab2);
					W.at<double>(i * rows + j, (i + 1) * rows + j) = ciede;
					sum += ciede;
				}
				if (j < orig.cols - 1)
				{
					bgr = orig.at<Vec3b>(j + 1);
					Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
					double ciede = CIEDE(lab1, lab2);
					W.at<double>(i * rows + j, i * rows + j + 1) = ciede;
					sum += ciede;
				}
				D.at<double>(i * rows + j, i * rows + j) = sum;
			}
		}

		//TODO: решить уравнение 
		//(D - W) * y = lambda * D * y
		//рекурсия?


		return orig;
	}
}

Mat meanShift(Mat orig)
{
	return Mat();
}
