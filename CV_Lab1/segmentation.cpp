#include "segmentation.h"

#include <Eigen/Core>
#include <Eigen/Sparse>

#include <GenEigsSolver.h>
#include <SymEigsSolver.h>
#include <SparseGenMatProd.h>
#include <SparseSymMatProd.h>


const double k = 0.005;

//const double k = 0.000005;


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

	//������������� ����� - ������
	//������������� ������ - '
	//d - ������

	double kL, kC, kH;
	kL= kC = kH = 1;

	double dL_ = L2 - L1;
	double _L = (L1 + L2) / 2;

	double c1 = sqrt(a1*a1 + b1 * b1);
	double c2 = sqrt(a2*a2 + b2 * b2);
	double _c = (c1 + c2) / 2; //c � �������

	double a1_ = a1 + a1 / 2 * (1 - sqrt(pow(_c, 7) / (pow(_c, 7) + pow(25, 7)))); //a1'
	double a2_ = a2 + a2 / 2 * (1 - sqrt(pow(_c, 7) / (pow(_c, 7) + pow(25, 7)))); //a1'
	double c1_ = sqrt(a1_ * a1_ + b1 * b1);
	double c2_ = sqrt(a2_ * a2_ + b2 * b2);
	double _c_ = (c1_ + c2_) / 2; //c' � �������
	double dC_ = c2_ - c1_; //delta c'


	double h1_ = atan2(b1, a1_);
	h1_ = (h1_ + M_PI) / M_PI * 180; //���������
	double h2_ = atan2(b2, a2_);
	h2_ = (h2_ + M_PI) / M_PI * 180;
	double dh_; //delta h_

	if (abs(h1_ - h2_) <= 180)
		dh_ = h2_ - h1_;
	else if (abs(h1_ - h2_) > 180 && h2_ <= h1_)
		dh_ = h2_ - h1_ + 360;
	else
		dh_ = h2_ - h1_ - 360;


	double dH_; //H �������!
	dH_ = 2 * sqrt(c1_ * c2_) * sin(dh_ / 2);
	//_H_ //H' � �������
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
	if (region.cols * region.rows != 1)
		return dev / (region.cols * region.rows - 1);
	else
		return dev;

	//��� �������� > 5 �� �����, ���������� ("���� � ������ ������") 
	//dist - ������
	//exp(-dist(c(x), c(y) ^ 2    / w * sigma ^ 2))
	//����� ����������� ������
}

double homogeneity(Mat region, uint iter)
{
	/*if (iter > 5 || iter = -1)
		return homogeneityRGB(region);
	else*/
		return homogeneity(getLMatrix(region));
}

Vec3b meanRGB(Mat region)
{
	long sumx = 0, sumy = 0, sumz = 0;
	uint count = region.rows * region.cols;
	for (int i = 0; i < region.rows; i++)
		for (int j = 0; j < region.cols; j++)
		{

			Vec3b color = region.at<Vec3b>(i, j);
			sumx += color[0];
			sumy += color[1];
			sumz += color[2];
		}
	Vec3b newcolor(sumx / count, sumy / count, sumz / count);
	return newcolor;
}

double homogeneityRGB(Mat region)
{
	long double dev = 0;
	Vec3b avg = meanRGB(region);
	double sigma = 1;
	for (int i = 0; i < region.rows; i++)
		for (int j = 0; j < region.cols; j++)
		{
			Vec3b color = region.at<Vec3b>(i, j);
			double eucl = //sqrt(
				pow(avg[0]-color[0], 2) + pow(avg[1] - color[1], 2) + pow(avg[2] - color[2], 2)  //)
			;
			//dev += eucl;
			double similarity = exp(-eucl/2 * sigma * sigma);
			//std::cout << similarity << std::endl;
			dev += similarity;

		}

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

	/*Mat L = getLMatrix(orig);
	Region r(L);*/
	Region r(orig);
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
	
	if (homogeneity(regmat, iterNum) > k) //� ����������� �� ������ �������� ������������ ������ �������: �� ������ ���� ����� �� L, ����� - CIEDE2000
	{
		region.addChild(Region(regmat(Rect(0, 0, cols / 2, rows / 2))));								
		region.addChild(Region(regmat(Rect(cols / 2, 0, cols - cols / 2, rows / 2))));
		region.addChild(Region(regmat(Rect(0, rows / 2, cols / 2, rows - rows / 2))));
		region.addChild(Region(regmat(Rect(cols / 2, rows / 2, cols - cols / 2, rows - rows / 2))));


		/*Mat qw(rows/2, cols, regmat.type());
		qw(Rect(0, 0, cols/2, rows/2)) = regmat(Rect(0, 0, cols / 2, rows / 2));
		qw(Rect(0, 0, cols - cols / 2, rows/2)) = regmat(Rect(cols / 2, 0, cols - cols / 2, rows / 2));
		regmat(Rect(cols / 2, 0, cols - cols / 2, rows / 2)) = 0;*/

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
		region.mat = meanRGB(regmat); //�� �� ��� ������� �����������
	}
}

void merge(Region &region)
{
	//���������� �� ������ ������
	Mat		hmerged1, hmerged2, vmerged1, vmerged2;
	bool	hb1, hb2, vb1, vb2;

	//Mat		hmerged1(region.children[0].mat.rows, region.children[0].mat.cols + region.children[1].mat.cols, region.mat.type());
	//!
	if (region.children.size() == 0)
		return;

	hconcat(region.children[0].mat, region.children[1].mat, hmerged1);
	//hb1 = homogeneityRGB(hmerged1) < k;
	hb1 = homogeneity(hmerged1, -1) < k;
	hconcat(region.children[2].mat, region.children[3].mat, hmerged2);
	hb2 = homogeneity(hmerged2, -1) < k;

	int cols = region.mat.cols;
	int rows = region.mat.rows;
	
	if (hb1 && !hb2)
	{
		region.children.erase(region.children.begin());
		region.children.erase(region.children.begin());
		Mat newmat = region.mat(Rect(0, 0, cols, rows / 2));
		region.children.push_back(Region(newmat));

	}
	else if (!hb1 && hb2)
	{
		region.children.erase(region.children.end() - 1);
		region.children.erase(region.children.end() - 1);
		Mat newmat = region.mat(Rect(0, rows/2, cols, rows - rows / 2));
		region.children.push_back(Region(newmat));

	}
	else if (hb2 && hb1)
	{
		region.children.erase(region.children.begin());
		region.children.erase(region.children.begin());
		region.children.erase(region.children.begin());
		region.children.erase(region.children.begin());
		Mat newmat1 = region.mat(Rect(0, 0, cols, rows / 2));
		region.children.push_back(Region(newmat1));
		Mat newmat2 = region.mat(Rect(0, rows / 2, cols, rows - rows / 2));
		region.children.push_back(Region(newmat2));
	}
	/*else //!hb1 && !hb2
	{
		vconcat(region.children[0].mat, region.children[2].mat, vmerged1);
		vb1 = homogeneity(vmerged1, -1) < k;
		vconcat(region.children[1].mat, region.children[3].mat, vmerged2);
		vb2 = homogeneity(vmerged2, -1) < k;
		

		if (vb1 && !vb2)
		{
			Region newregion(region.children[0]);
			newregion.addMat = region.children[2].mat;
			region.children.erase(region.children.begin());
			region.children.erase(region.children.begin() + 1);
			region.children.push_back(newregion);
		}
		else if (!vb1 && vb2)
		{
			Region newregion(region.children[1]);
			newregion.addMat = region.children[3].mat;
			region.children.erase(region.children.begin() + 1);		//ft
			region.children.erase(region.children.end() - 1);		//ft	//ftft - > fft
			region.children.push_back(newregion);
		}
		else if (vb2 && vb1)
		{
			Region newregion1(region.children[0]);
			newregion1.addMat = region.children[2].mat;
			Region newregion2(region.children[1]);
			newregion2.addMat = region.children[3].mat;
			region.children.erase(region.children.begin()); 
			region.children.erase(region.children.begin());
			region.children.erase(region.children.begin());
			region.children.erase(region.children.begin());
			region.children.push_back(newregion1);
			region.children.push_back(newregion2);
		}
	}*/

}

Mat normalizedCut(Mat orig)
{
	Mat mask(orig.rows, orig.cols, CV_8UC1);
	mask = 1;
	Mat copy;
	orig.copyTo(copy);
	normalizedCut(copy, mask, 1);
	return copy;
}

void normalizedCut(Mat &orig, Mat mask, uint iter)
{
	std::cout << "Iteration: " << iter << std::endl;
	if (iter == 0)
	{
		fillWithMean(orig, mask);
		std::cout << "Enough" << std::endl;
		return;
	}

	Mat mask1(orig.rows, orig.cols, CV_8UC1), mask2(orig.rows, orig.cols, CV_8UC1);

	unsigned long rowscols = orig.rows * orig.cols;
	Eigen::SparseMatrix<float> W_(rowscols, rowscols);
	W_.reserve(Eigen::VectorXi::Constant(orig.rows * orig.cols, 9)); //5 - ���� � ������ 4-��������� (4 + 1 ����.) ������ - 9 
	uint cols = orig.cols;
	std::vector<float> D(orig.rows * orig.cols, 0);


	for (int i = 0; i < orig.rows; i++)
	{
		for (int j = 0; j < orig.cols; j++)
		{
			//� ��� ������, ��� ��� ����� ���������� ������������ ������� �� ����� ����� ����?
			//�� ��� ����� ���������?
			//� ������, ����� � �� ����� ��� ������
			//�����
			//������ �� �������� �� �������� ����� ������� �������� ������ ������� ����������!
			//�� ��� ���� ���-�� ������...
			if (mask.at<uchar>(i, j) != 0)
			{
				Vec3b bgr = orig.at<Vec3b>(i, j);
				Vec3d lab1 = getLab(bgr[2], bgr[1], bgr[0]);
				float sum = 0;
				if (i >= 1)
				{

					bgr = orig.at<Vec3b>(i - 1, j);
					Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
					float ciede = CIEDE(lab1, lab2);
					//W.ref<float>(i * cols + j, (i - 1) * cols + j) = -ciede;	//� ������� ��� D - W
					W_.insert(i * cols + j, (i - 1) * cols + j) = -ciede;
					sum += ciede;

					if (j >= 1)
					{

						bgr = orig.at<Vec3b>(i - 1, j - 1);
						Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
						float ciede = CIEDE(lab1, lab2);
						W_.insert(i * cols + j, (i - 1) * cols + j - 1) = -ciede;
						sum += ciede;
					}

					if (j < orig.cols - 1)
					{

						bgr = orig.at<Vec3b>(i - 1, j + 1);
						Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
						float ciede = CIEDE(lab1, lab2);
						W_.insert(i * cols + j, (i - 1) * cols + j + 1) = -ciede;
						sum += ciede;
					}

				}
				if (j >= 1)
				{

					bgr = orig.at<Vec3b>(i, j - 1);
					Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
					float ciede = CIEDE(lab1, lab2);
					W_.insert(i * cols + j, i * cols + j - 1) = -ciede;
					sum += ciede;
				}
				if (i < orig.rows - 1)
				{

					bgr = orig.at<Vec3b>(i + 1, j);
					Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
					float ciede = CIEDE(lab1, lab2);
					W_.insert(i * cols + j, (i + 1) * cols + j) = -ciede;
					sum += ciede;

					if (j >= 1)
					{
						bgr = orig.at<Vec3b>(i + 1, j - 1);
						Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
						float ciede = CIEDE(lab1, lab2);
						W_.insert(i * cols + j, (i + 1) * cols + j - 1) = -ciede;
						sum += ciede;
					}
					if (j < orig.cols - 1)
					{
						bgr = orig.at<Vec3b>(i + 1, j + 1);
						Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
						float ciede = CIEDE(lab1, lab2);
						W_.insert(i * cols + j, (i + 1) * cols + j + 1) = -ciede;
						sum += ciede;
					}
				}
				if (j < orig.cols - 1)
				{

					bgr = orig.at<Vec3b>(i, j + 1);
					Vec3d lab2 = getLab(bgr[2], bgr[1], bgr[0]);
					float ciede = CIEDE(lab1, lab2);
					W_.insert(i * cols + j, i * cols + j + 1) = -ciede;
					sum += ciede;
				}
				D.at(i * cols + j) = sum;
				//std::cout << count << " "; //�������� - 8 �� ��
			}
		}
	}
	/*std::cout << W_ << std::endl;
	for (auto i : D)
		std::cout << i << " ";
	std::cout << std::endl;*/
	//(D - W) * y = lambda * D * y
	std::cout << "W and D are filled" << std::endl;
	//Mat eigenMat = D.inv() * (D - W);	//D^-1 * (D - W)

	for (int i = 0; i < rowscols; i++)
	{
		//W.ref<float>(i, i) += D.at(i);
		W_.insert(i, i) = D[i];
	}
	std::cout << "Found W := D - W" << std::endl;
	//std::cout << W_ << std::endl;
	//!!! D^-1 * (D - W): ������ ��� ����� �� d1 ������ �� d2 � �.� !!!
	for (int k = 0; k < W_.outerSize(); k++)
	{
		for (Eigen::SparseMatrix<float>::InnerIterator it(W_, k); it; ++it)
		{
			if(D[it.row()] != 0)		//???
				it.valueRef() /= D[it.row()];
			//std::cout << it.row() << std::endl; //����� row? �������
		}
	}
	std::cout << "Found D^-1 * (D - W)" << std::endl; 
	//std::cout << W_ << std::endl;
	/*for (int k = 0; k < W_.outerSize(); k++)
	{
		for (Eigen::SparseMatrix<float>::InnerIterator it(W_, k); it; ++it)
		{
			if (D[it.row()] != 0)		//???
				it.valueRef() /= sqrt(D[it.row()]);
			if (D[it.col()] != 0)		//???
				it.valueRef() /= sqrt(D[it.col()]);
			//std::cout << it.row() << std::endl; //����� row? �������
		}
	} //����� ������ � �������? ���������!!!!
	std::cout << "Found D^-1/2 * (D - W) * D^-1/2" << std::endl; //it is symmetric*/


	Spectra::SparseGenMatProd<float> op(W_);
	Spectra::SparseSymMatProd<float> symOp(W_);
	Spectra::GenEigsSolver<float, Spectra::SMALLEST_REAL, Spectra::SparseGenMatProd<float>> eigs(&op, 2, 50); //����� �� �������� ��� ���������� ��������
	//Spectra::SymEigsSolver<float, Spectra::SMALLEST_ALGE, Spectra::SparseSymMatProd<float>> eigs(&symOp, 2, 50); //����� �� �������� ��� ���������� ��������

	eigs.init();
	const auto nconv = eigs.compute(2000, 1e-3);
	std::cout << "Converged eigenvalues: " << nconv << std::endl;

	//���������� ������ ������ ������ ����� ����� ����
	Mat ret(orig.rows, orig.cols, orig.type());
	if (eigs.info() == Spectra::SUCCESSFUL)
	{
		const auto evalues = eigs.eigenvalues();
		const auto evectors = eigs.eigenvectors(); //eigen::MatrixXcf

		const auto evector = evectors.col(0); //����� �� ������������� ���, ��� ������� ���� �������, ������� ����� �������
		//�� �� ����� ���� col(0) - ������� ������ ��������	
		//���� ����� col(1), �� ��� �����������..
		std::cout << evalues << std::endl;
		std::cout << evectors << std::endl;
		for (int j = 0; j < rowscols; j++)
		{
		int y = j / orig.cols;
		int x = j % orig.cols;
		if (evector[j].real() < 0)
		{
			mask1.at<uchar>(y, x) = 255;
			mask2.at<uchar>(y, x) = 0;
		}
		else
		{
			mask1.at<uchar>(y, x) = 0;
			mask2.at<uchar>(y, x) = 255;
		}
		}
		imshow("MASK", mask1);
		iter--;
		normalizedCut(orig, mask1, iter);
		normalizedCut(orig, mask2, iter);
	}
}

void fillWithMean(Mat & orig, Mat mask)
{
	//Vec3b sum = 0;
	int sumx = 0, sumy = 0, sumz = 0;
	int count = 0;
	for (int i = 0; i < orig.rows; i++)
	{
		for (int j = 0; j < orig.cols; j++)
		{

			if ((uint)mask.at<uchar>(i, j) == 255)
			{
				count++;
				Vec3b color = orig.at<Vec3b>(i, j);
				sumx += color[0];
				sumy += color[1];
				sumz += color[2];
			}
		}
	}
	sumx /= count;
	sumy /= count;
	sumz /= count;
	Vec3b nc(sumx, sumy, sumz);
	for (int i = 0; i < orig.rows; i++)
	{
		for (int j = 0; j < orig.cols; j++)
		{
			if (mask.at<uchar>(i, j) == 255)
			{
				orig.at<Vec3b>(i, j) = nc;
			}
		}
	}

}


Mat meanShift(Mat orig, int spatialRadius, double colorRadius)
{
	const int maxStep = 5;
	const int colorStop = 0.3;
	const int spatialStop = 0.3;
	Mat copy, ret;
	orig.copyTo(copy);

	int cols = orig.cols;
	int rows = orig.rows;

	Point5D curr, prev;

	for (int i = 0; i < orig.rows; i++)
	{
		for (int j = 0; j < orig.cols; j++)
		{
			int left = (j - spatialRadius) >= 0 ? (j - spatialRadius) : 0;
			int top = (i - spatialRadius) >= 0 ? (i - spatialRadius) : 0;
			int right = (j + spatialRadius) < cols ? (j + spatialRadius) : cols;
			int bottom = (i + spatialRadius) < rows ? (i + spatialRadius) : rows;

			Vec3b bgr = orig.at<Vec3b>(i, j);
			//space.push_back(Point5D(j, i, bgr[0], bgr[1], bgr[2]));

			curr.setPoint(i, j, bgr[0], bgr[1], bgr[2]);
			//std::cout << "here" << std::endl;
			int step = 0;
			do
			{
				prev.setPoint(curr);
				Point5D psum(0, 0, 0, 0, 0);
				int count = 0;

				for (int x = top; x < bottom; x++)
				{
					for (int y = left; y < right; y++)
					{
						Vec3b wbgr = orig.at<Vec3b>(x, y);
						Point5D wPoint(x, y, wbgr[0], wbgr[1], wbgr[2]);
						if (wPoint.colorDistance(curr) < colorRadius)
						{
							psum.addPoint(wPoint);
							
							count++;
						}
					}
				}
				//std::cout << count << std::endl;
				curr.setDiv(psum, count);
				step++;
			} while (curr.colorDistance(prev) > colorStop && curr.spatialDistance(prev) > spatialStop && step < maxStep);
			
			copy.at<Vec3b>(i, j) = Vec3b(curr.b, curr.g, curr.r);
		}
	}


	//segmentation
	int curlabel = -1;
	//char * regions = new char[rows * cols * 3];
	std::vector<Vec3i> regions;
	//std::vector<int> regmembercount;
	int ** labels = new int *[rows];
	for (int i = 0; i < rows; i++)
	{
		labels[i] = new int[cols];
		for (int j = 0; j < cols; j++)
			labels[i][j] = -1;
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (labels[i][j] < 0) //�� ���� ��� �� ��������
			{
				int membercount = 0;
				labels[i][j] = ++curlabel;
				Vec3b bgr = orig.at<Vec3b>(i, j);
				curr.setPoint(i,j,bgr[0], bgr[1], bgr[2]);
				regions.push_back(Vec3i(curr.b, curr.g, curr.r)); //regions[label]

				std::vector<Point5D> neighbours;
				neighbours.push_back(curr);

				while (!neighbours.empty())
				{
					Point5D point = neighbours.back();
					neighbours.pop_back();

					for (int x = -1; x < 2; x++) //������
					{
						for (int y = -1; y < 2; y++)
						{
							if (x != 0 || y != 0)
							{
								int hx = point.x + x;
								int hy = point.y + y;
								if (hx >= 0 && hy >= 0 && hx < rows && hy < cols && labels[hx][hy] < 0)
								{
									Vec3b bgr = orig.at<Vec3b>(hx, hy);
									Point5D wPoint(hx, hy, bgr[0], bgr[1], bgr[2]);

									if (curr.colorDistance(wPoint) < colorRadius)
									{
										labels[hx][hy] = curlabel;
										neighbours.push_back(wPoint);

										regions[curlabel] += bgr;
										membercount++;
									}
								}
							}
						}
					}
				}
				membercount++;
				regions[curlabel] /= membercount;
			}
		}
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int label = labels[i][j];
			copy.at<Vec3b>(i, j) = regions[label];
		}
	}



	return copy;
}
