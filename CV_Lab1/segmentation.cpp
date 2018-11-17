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
	//���������� �� ������ ������
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

//�����: ������ ���� ����� ������ �������, ������  - �� ������ �������� ������� � ������! TODO


Mat normalizedCut(Mat orig)
{
	Mat W(orig.rows + orig.cols, orig.rows + orig.cols, CV_64FC1);
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
				bgr = orig.at<Vec3b>(i-1, j);
				Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
				double ciede = CIEDE(lab1, lab2);
				W.at<double>(i * rows + j, (i-1) * rows + j) = ciede;
				sum += ciede;
				//�����, ����� ���-�� ����� ��� (j,i) � ��������� ����?
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
			D.at<double>(i * rows + i, i * rows + i) = sum;
		}
	}

	return orig;
}