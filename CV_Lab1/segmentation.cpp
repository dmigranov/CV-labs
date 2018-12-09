#include "segmentation.h"

#include <Eigen/Core>
#include <Eigen/Sparse>

#include <GenEigsSolver.h>
#include <SymEigsSolver.h>
#include <SparseGenMatProd.h>
#include <SparseSymMatProd.h>


const double k = 0.01;

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

	//подчЄркивание слева - крышка
	//подчЄркивание справа - '
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
	if (region.cols * region.rows != 1)
		return dev / (region.cols * region.rows - 1);
	else
		return dev;

	//дл€ итераци€ > 5 по цвету, экспонента ("было в первой лекции") 
	//dist - евклид
	//exp(-dist(c(x), c(y) ^ 2    / w * sigma ^ 2))
	//сигма подбираетс€ руками
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
	std::cout << "BEGIN" << std::endl;
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
			std::cout << similarity << std::endl;
			dev += similarity;

		}
	std::cout << "END" << std::endl;

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
	std::cout << homogeneity(regmat, iterNum) << std::endl;
	if (homogeneity(regmat, iterNum) > k) //в зависимости от номера итерации использовать разные формулы: на первых п€ти можно по L, далее - CIEDE2000
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
		region.mat = meanRGB(regmat);
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
	//hb1 = homogeneityRGB(hmerged1) < k;
	hb1 = homogeneity(hmerged1, -1) < k;
	hconcat(region.children[2].mat, region.children[3].mat, hmerged2);
	hb2 = homogeneity(hmerged2, -1) < k;
	
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
		vb1 = homogeneity(vmerged1, -1) < k;
		vconcat(region.children[1].mat, region.children[3].mat, vmerged2);
		vb2 = homogeneity(vmerged2, -1) < k;
		

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
	W_.reserve(Eigen::VectorXi::Constant(orig.rows * orig.cols, 9)); //5 - было в случае 4-св€зности (4 + 1 диаг.) сейчас - 9 
	uint cols = orig.cols;
	std::vector<float> D(orig.rows * orig.cols, 0);


	for (int i = 0; i < orig.rows; i++)
	{
		for (int j = 0; j < orig.cols; j++)
		{
			//а кто сказал, что при таком заполнении дискриминант матрицы не будет равен нулю?
			//но как тогда заполн€ть?
			//и кстати, зачем € ее много раз считаю
			//—“ќѕЁ
			//вообще по хорошему на проходах после первого мен€етс€ размер матрицы смежностей!
			//но это тоже как-то сложно...
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
					//W.ref<float>(i * cols + j, (i - 1) * cols + j) = -ciede;	//с минусом ибо D - W
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
				//std::cout << count << " "; //максимум - 8 всЄ ок
			}
		}
	}
	//(D - W) * y = lambda * D * y
	std::cout << "W and D are filled" << std::endl;
	//Mat eigenMat = D.inv() * (D - W);	//D^-1 * (D - W)

	for (int i = 0; i < rowscols; i++)
	{
		//W.ref<float>(i, i) += D.at(i);
		W_.insert(i, i) = D[i];
	}
	std::cout << "Found W := D - W" << std::endl;
	//!!! D^-1 * (D - W): первый р€д делим на d1 второй на d2 и т.д !!!
	/*for (int k = 0; k < W_.outerSize(); k++)
	{
		for (Eigen::SparseMatrix<float>::InnerIterator it(W_, k); it; ++it)
		{
			if(D[it.row()] != 0)		//???
				it.valueRef() /= D[it.row()];
			//std::cout << it.row() << std::endl; //точно row? наверно
		}
	}
	std::cout << "Found D^-1 * (D - W)" << std::endl; */

	for (int k = 0; k < W_.outerSize(); k++)
	{
		for (Eigen::SparseMatrix<float>::InnerIterator it(W_, k); it; ++it)
		{
			if (D[it.row()] != 0)		//???
				it.valueRef() /= sqrt(D[it.row()]);
			if (D[it.col()] != 0)		//???
				it.valueRef() /= sqrt(D[it.col()]);
			//std::cout << it.row() << std::endl; //точно row? наверно
		}
	} //может ошибка в делении? ѕ–ќ¬≈–»“№!!!!
	std::cout << "Found D^-1/2 * (D - W) * D^-1/2" << std::endl; //it is symmetric


	Spectra::SparseGenMatProd<float> op(W_);
	Spectra::SparseSymMatProd<float> symOp(W_);
	//Spectra::GenEigsSolver<float, Spectra::SMALLEST_REAL, Spectra::SparseGenMatProd<float>> eigs(&op, 2, 50); //вроде бы выбирает два наименьших значени€
	Spectra::SymEigsSolver<float, Spectra::SMALLEST_ALGE, Spectra::SparseSymMatProd<float>> eigs(&symOp, 2, 50); //вроде бы выбирает два наименьших значени€

	eigs.init();
	const auto nconv = eigs.compute(2000, 1e-3);
	std::cout << "Converged eigenvalues: " << nconv << std::endl;

	//наименьший собств вектор скорее всего будет ноль
	Mat ret(orig.rows, orig.cols, orig.type());
	if (eigs.info() == Spectra::SUCCESSFUL)
	{
		const auto evalues = eigs.eigenvalues();
		const auto evectors = eigs.eigenvectors(); //eigen::MatrixXcf
		const auto evector = evectors.col(1); //вроде бы отсортированы так, что сначала идут бќльшие, поэтому нужен нулевой
		//но на самом деле col(0) - нулевое собств значение		
		std::cout << evectors << std::endl;
		for (int j = 0; j < rowscols; j++)
		{
			int y = j / orig.cols;
			int x = j % orig.cols;
			if (evector[j] < -0.009)
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




Mat meanShift(Mat orig)
{
	Mat copy, ret;
	orig.copyTo(copy);


	return Mat();
}
