#include "filtration.h"
#include "lab.h"



Mat gauss_filter(Mat original, double sigma)
{
	//5x5 filter

	Mat orig;
	original.convertTo(orig, CV_64F);
	Mat gauss(5, 5, CV_64FC1);
	Mat newimg(orig.rows, orig.cols, orig.type());
	newimg = 0;
	double div = 0;
	for (int x = -2; x < 3; x++)
		for (int y = -2; y < 3; y++)
		{
			gauss.at<double>(x + 2, y + 2) = exp(-(x * x + y * y) / (2.0 * sigma * sigma)) / (2.0 * M_PI * sigma * sigma); //not simmetric! 5
			div += gauss.at<double>(x + 2, y + 2);
		}
	gauss /= div;

	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			for (int x = -2; x < 3; x++)
				for (int y = -2; y < 3; y++)
				{
					if (i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols)
					{
						if(orig.channels() == 3)
							newimg.at<Vec3d>(i, j) += orig.at<Vec3d>(i + x, j + y) * gauss.at<double>(x + 2, y + 2);
						else if (orig.channels() == 1)
							newimg.at<double>(i, j) += orig.at<double>(i + x, j + y) * gauss.at<double>(x + 2, y + 2);
					}
				}
		}
	newimg.convertTo(newimg, original.type());
	return newimg;

}


Mat sobel_filter(Mat orig, Mat * grad)
{
	//

	Mat Gx = (Mat_<int>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	Mat Gy = (Mat_<int>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
	

	Mat labimg;

	/*cvtColor(orig, labimg, COLOR_BGR2Lab);
	std::vector<Mat> labv;
	split(labimg, labv);
	Mat L;
	labv[0].convertTo(L, CV_64FC1);
	L /= 255.0;*/ //the commented segment uses OpenCV's convertion

	Mat L = getLMatrix(orig); //my convertion

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
					if (i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols)
					{
						gxv +=
							L.at<double>(i + x, j + y) * Gx.at<int>(x + 1, y + 1);
						gyv += 
							L.at<double>(i + x, j + y) * Gy.at<int>(x + 1, y + 1);
					}
				}
			newimg.at<double>(i, j) = sqrt(gxv * gxv + gyv * gyv);
			if (grad != NULL)
			{
				double tan = atan2(gyv, gxv); //atan: -pi/2; +pi/2 //atan2: -pi, pi
				if (tan < 0) tan += M_PI;

				if (tan <= M_PI / 8 || tan > 7 * M_PI / 8)
					tan = 0;
				if (tan > M_PI / 8 && tan <= 3 * M_PI / 8)
					tan = M_PI / 4;
				if (tan > 3 * M_PI / 8 && tan <= 5 * M_PI / 8)
					tan = M_PI / 2;
				if (tan > 5 * M_PI / 8 && tan <= 7 * M_PI / 8)
					tan = 3 * M_PI / 4;
				grad->at<double>(i, j) = tan;
			}
		}

	return newimg;
}

double doubleTreshold(double resV, double lower, double upper)
{
	if (resV > upper)
		return 1;
	else if (resV < lower)
		return 0;
	else
		return 0.5;
}


Mat canny(Mat orig, double lower, double upper)
{
	//Mat res = ;
	Mat grad(orig.rows, orig.cols, CV_64FC1);
	Mat res = sobel_filter(gauss_filter(orig, 0.5), &grad);
	Mat newres;
	res.copyTo(newres);

	//non-maximum suppression + двойная пороговая фильтрация
	for (int i = 0; i < grad.rows; i++)
	{
		for (int j = 0; j < grad.cols; j++)
		{
			double gradV = grad.at<double>(i, j);
			double resV = res.at<double>(i, j);
			if (gradV == M_PI / 2 && i > 0 && i < grad.rows - 1)
			{
				if (!(resV >= res.at<double>(i - 1, j) && resV >= res.at<double>(i + 1, j)))
				{
					newres.at<double>(i, j) = 0;
				}
				else 
				{
					newres.at<double>(i, j) = doubleTreshold(resV, lower, upper);
				}
			}
			else if (gradV == 0 && j > 0 && j < grad.cols - 1)
			{
				if (!(resV >= res.at<double>(i, j - 1) && resV >= res.at<double>(i, j + 1)))
				{
					newres.at<double>(i, j) = 0;
				}
				else
				{
					newres.at<double>(i, j) = doubleTreshold(resV, lower, upper);
				}
			}
			else if (gradV == M_PI / 4 && i > 0 && j < grad.cols - 1 && i < grad.rows - 1 && j > 0)
			{
				if (!(resV >= res.at<double>(i - 1, j - 1) && resV >= res.at<double>(i + 1, j + 1)))
				{
					newres.at<double>(i, j) = 0;
				}
				else
				{
					newres.at<double>(i, j) = doubleTreshold(resV, lower, upper);
				}
			}

			else if (gradV == 3 * M_PI / 4 && i > 0 && j < grad.cols - 1 && i < grad.rows - 1 && j > 0)
			{
				if (!(resV >= res.at<double>(i + 1, j - 1) && resV >= res.at<double>(i - 1, j + 1)))
				{
					newres.at<double>(i, j) = 0;
				}
				else
				{
					newres.at<double>(i, j) = doubleTreshold(resV, lower, upper);
				}
			}
		}
	}


	//продолжение кривых (да, оно не идеально по определению алгоритма; идём стандартной змейкой)
	for (int i = 0; i < grad.rows; i++)
	{
		for (int j = 0; j < grad.cols; j++)
		{
			if (newres.at<double>(i, j) == 0.5)
			{
				if (i > 0 && newres.at<double>(i - 1, j) == 1)
				{
					newres.at<double>(i, j) = 1;
					continue;
				}
				if (j > 0 && newres.at<double>(i, j - 1) == 1)
				{
					newres.at<double>(i, j) = 1;
					continue;
				}
				if (j > 0 && i > 0 && newres.at<double>(i - 1, j - 1) == 1)
				{
					newres.at<double>(i, j) = 1;
					continue;
				}
				if (i < grad.rows - 1 && newres.at<double>(i + 1, j) == 1)
				{
					newres.at<double>(i, j) = 1;
					continue;
				}
				if (j < grad.cols - 1 && newres.at<double>(i, j + 1) == 1)
				{
					newres.at<double>(i, j) = 1;
					continue;
				}
				if (i > 0 && j < grad.cols - 1 && newres.at<double>(i - 1, j + 1) == 1)
				{
					newres.at<double>(i, j) = 1;
					continue;
				}
				if (j > 0 && i < grad.rows - 1 && newres.at<double>(i + 1, j - 1) == 1)
				{
					newres.at<double>(i, j) = 1;
					continue;
				}
				if (j < grad.cols && i < grad.rows - 1 && newres.at<double>(i + 1, j + 1) == 1)
				{
					newres.at<double>(i, j) = 1;
					continue;
				}
				newres.at<double>(i, j) = 0;
			}
		}
	}


	return newres;
}

Mat gabor_filter(Mat orig, double theta, double phi, double sigma, double gamma, double lambda) //тета - угол; sigma / lambda примерно равно 0.56 (почему?)
{
	Mat gabor(5, 5, CV_64FC1);
	Mat L;
	if (orig.channels() == 1)
		L = orig;
	else
		L = getLMatrix(orig);
	Mat newimg(orig.rows, orig.cols, L.type());
	newimg = 0;
	double div = 0;
	for (int x = -2; x < 3; x++)
		for (int y = -2; y < 3; y++)
		{
			double x_ =  x * cos(theta) + y * sin(theta);
			double y_ = -x * sin(theta) + y * cos(theta);

			gabor.at<double>(x + 2, y + 2) = exp(-(x_ * x_ + gamma * gamma * y_ * y_) / (2.0 * sigma * sigma)) * cos(2 * M_PI * x_ / lambda + phi);//not simmetric! 5
			div += gabor.at<double>(x + 2, y + 2);
		}
	gabor /= div;

	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			for (int x = -2; x < 3; x++)
				for (int y = -2; y < 3; y++)
				{
					if (i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols)
					{
						newimg.at<double>(i, j) += L.at<double>(i + x, j + y) * gabor.at<double>(x + 2, y + 2);
					}
				}
		}

	//std::cout << newimg << std::endl;



	return newimg;
}


Mat otsu(Mat orig)
{
	Mat L;
	if (orig.channels() == 1)
		L = orig;
	else
		L = getLMatrix(orig);
	
	double p[101] = { 0 }, N[101];
	double Nt = L.cols * L.rows;
	int all = 0;
	//гистограмма
	//самый тупой способ
	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			//std::cout << L.at<double>(i, j) * 100 << std::endl;
			p[(int)(L.at<double>(i, j) * 100)]++;
		}
	//summ(p[i]) = all = L.rows * L.cols!


	double uT = 0;
	for (int i = 0; i < 101; i++)
	{
		N[i] = p[i] / Nt;
		uT += i * N[i];
	}
	double w1 = 0, w2, uSum = 0, u1, u2, max = 0;
	int maxT;
	//w1(0), ... ?
	for (int t = 1; t < 101; t++) //1?
	{
		
		w1 += N[t];
		w2 = 1 - w1;

		uSum += t * N[t];
		u1 = uSum / w1;
		u2 = (uT - u1 * w1) / w2;
		double disp = w1 * w2 * pow(u1 - u2, 2);
		if (disp > max)
		{
			max = disp;
			maxT = t;
		}
	}

	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			//std::cout << L.at<double>(i, j) * 100 << std::endl;
			if (L.at<double>(i, j) > maxT)
				L.at<double>(i, j) = 1;
			else
				L.at<double>(i, j) = 0;
		}


	//std::cout << all << " " << L.rows * L.cols << std::endl;

	return L;

}