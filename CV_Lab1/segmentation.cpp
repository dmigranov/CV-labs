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


//double euclidean_distance(Vec3d )

double CIEDE2000(Vec3d Lab1, Vec3d Lab2)
{
	double L2, L1, a2, a1, b2, b1;
	L2 = Lab2[0];
	L1 = Lab1[0];
	a2 = Lab2[1];
	a1 = Lab1[1];
	b2 = Lab2[2];
	b1 = Lab1[2];

	//подчёркивание слева - крышка
	//подчёркивание справа - '
	//d - дельта

	double kL, kC, kH;
	kL= kC = kH = 1;

	double dL_ = L2 - L1;
	double _L = (L1 + L2) / 2;

	double c1 = sqrt(a1*a1 + b1 * b1);
	double c2 = sqrt(a2*a2 + b2 * b2);
	double _c = (c1 + c2) / 2; //c с крышкой

	double a1_ = a1 + a1 / 2 * (1 - sqrt(pow(_c, 7) / (pow(_c, 7) + pow(25, 7)))); //a1'
	double a2_ = a2 + a2 / 2 * (1 - sqrt(pow(_c, 7) / (pow(_c, 7) + pow(25, 7)))); //a1'
	double c1_ = sqrt(a1_ * a1_ + b1 * b1);
	double c2_ = sqrt(a2_ * a2_ + b2 * b2);
	double _c_ = (c1_ + c2_) / 2; //c' с крышкой
	double dC_ = c2_ - c1_; //delta c'


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
	double _H_;
	if (abs(h1_ - h2_) <= 180)
		_H_ = (h2_ + h1_) / 2;
	else if (abs(h1_ - h2_) > 180 && h2_ + h1_ < 360)
		_H_ = (h2_ + h1_ + 360) / 2;
	else
		_H_ = (h2_ + h1_ - 360) / 2;


	double T = 1 - 0.17*cos(_H_ - 30) + 0.24 * cos(2 * _H_) + 0.32 * cos(3 * _H_ + 6) - 0.2 * cos(4 * _H_ - 63);
	double Sl = 1 + 0.015 * pow(_L - 50, 2)
					/
					sqrt(20 + pow(_L - 50, 2));
	double Sc = 1 + 0.045 * _c_;
	double Sh = 1 + 0.015 * _c_ * T;
	double Rt = -2 * sqrt(pow(_c_, 7) / (pow(_c_, 7) + pow(25, 7))) * sin(
	60 * exp( - pow( (_H_ - 275) / 25, 2) )
	);


	double dE = sqrt(
		pow(dL_ / (kL *Sl), 2)
		+
		pow(dC_ / (kC * Sc), 2)
		+
		pow(dH_ / (kH * Sh), 2)
		+
		Rt * dC_ / (kC * Sc) * dH_ / (kH * Sh)
	);

	return dE;
}

double CIEDE(Vec3d Lab1, Vec3d Lab2)
{
	//return CIE76(Lab1, Lab2);
	return CIEDE2000(Lab1, Lab2);
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

	if (homogeneity(regmat) > k) //в зависимости от номера итерации использовать разные формулы: на первых пяти можно по L, далее - CIEDE2000
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


Mat normalizedCut(Mat orig)
{
	
	const int sizes[] = { orig.rows * orig.cols, orig.rows * orig.cols };
	//Mat W(orig.rows * orig.cols, orig.rows * orig.cols, CV_32FC1);
	SparseMat W(2, sizes, CV_32FC1);
	//uint rows = orig.rows;
	uint cols = orig.cols;
	//Mat D(W.rows, W.cols, W.type());
	std::vector<float> D(orig.rows * orig.cols, 0);


	for (int i = 0; i < orig.rows; i++)
	{
		for (int j = 0; j < orig.cols; j++)
		{
			Vec3b bgr = orig.at<Vec3b>(i, j);
			Vec3d lab1 = getLab(bgr[2], bgr[1], bgr[0]);
			float sum = 0;
			if (i >= 1)
			{
				bgr = orig.at<Vec3b>(i - 1, j);
				Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
				float ciede = CIEDE(lab1, lab2);
				W.ref<float>(i * cols + j, (i - 1) * cols + j) = -ciede;	//с минусом ибо D - W
				sum += ciede;
			}
			//может, стоит как-то сразу для (j,i) и сократить цикл?
			if (j >= 1)
			{
				bgr = orig.at<Vec3b>(i, j - 1);
				Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
				float ciede = CIEDE(lab1, lab2);
				W.ref<float>(i * cols + j, i * cols + j - 1) = -ciede;
				sum += ciede;
			}
			if (i < orig.rows - 1)
			{
				bgr = orig.at<Vec3b>(i + 1);
				Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
				float ciede = CIEDE(lab1, lab2);
				W.ref<float>(i * cols + j, (i + 1) * cols + j) = -ciede;
				sum += ciede;
			}
			if (j < orig.cols - 1)
			{
				bgr = orig.at<Vec3b>(j + 1);
				Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
				float ciede = CIEDE(lab1, lab2);
				W.ref<float>(i * cols + j, i * cols + j + 1) = -ciede;
				sum += ciede;
			}
			D.at(i * cols + j) = sum;

		}
	}

	//(D - W) * y = lambda * D * y
	std::cout << "W and D are filled" << std::endl;

	//Mat eigenMat = D.inv() * (D - W);	//D^-1 * (D - W)
	//D.release();
	//W.release();


	//сейчас посчитаем D - W
	for (int i = 0; i < W.size(0); i++)
	{
		W.ref<float>(i, i) += D.at(i);

		/*for (int j = 0; j < W.size(1); j++)
		{
			W.ref<float>(i, j) /= D[i];
		}*/
	}
	std::cout << "Found W := D - W" << std::endl;

	//SparseMatIterator_ <float> it = W.begin<float>();
	//SparseMatIterator_ <float> it_end = W.end<float>();

	SparseMatIterator it = W.begin();
	SparseMatIterator it_end = W.end();

	int count = 0;
	for (; it != it_end; it++)
	{
		const SparseMat::Node * n = it.node();
		int node_i = n->idx[0];
		it.value<float>() /= D[node_i];
		count++;
	}

	std::cout << "Found W := D^(-1) * (D - W) " << count << std::endl;

	//TODO: D^-1 * (D - W):
	//первый ряд делим на d1
	//второй на d2 и т.д

	/*for (int i = 0; i < D.rows; i++)
	{
		if (D.at<float>(i, i) == 0)
			std::cout << i << std::endl;
	}*/

	
	/*Mat eigenvectors;
	Mat eigenvalues;
	eigen(eigenMat, eigenvalues, eigenvectors);
	//eigenvectors : vectors are stored in rows
	double min = 100000, oldmin = 100000;
	float * minptr = NULL, *oldminptr = NULL;

	for (int i = 0; i < eigenvectors.rows; i++)
	{
		float * row = eigenvectors.ptr<float>(i);
		double norm = 0;
		for (int j = 0; j < eigenvectors.cols; j++)
		{
			norm += row[j] * row[j];
		}
		if (norm < min)
		{
			oldmin = min;
			min = norm;
			oldminptr = minptr;
			minptr = row;
		}
	}

	for (int j = 0; j < eigenvectors.cols; j++)
	{
		int y = j / orig.cols;
		int x = j % orig.cols;
		if (oldminptr[j] < 0)
			ret.at<Vec3b>(y, x) = 0;
		else
			ret.at<Vec3b>(y, x) = Vec3b(255, 255,255);
	}*/
	return orig;
}
Mat meanShift(Mat orig)
{
	return Mat();
}
