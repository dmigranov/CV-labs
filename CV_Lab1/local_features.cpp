#include "local_features.h"


Mat harris_detector(Mat orig)
{
	Mat L = getLMatrix(orig) * 100;
	double k = 0.05; //k  [0.04, 0.06] //получать аргументом
	Mat Gx = (Mat_<int>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	Mat Gy = (Mat_<int>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);

	for (int i = 0; i < orig.rows; i++)
	{ 
		for (int j = 0; j < orig.cols; j++)
		{
			Mat M(2, 2, L.type());
			M = 0;
			Mat IMat(2, 2, L.type());
			double Ix = 0, Iy = 0; //средние по столбцу и строке (или наоборот?) хотя не важно		


			for (int x = -1; x < 2; x++)
				for (int y = -1; y < 2; y++)
					if (i + x >= 0 && i + x < L.rows && j + y >= 0 && j + y < L.cols)
					{
						Ix +=
							L.at<double>(i + x, j + y) * Gx.at<int>(x + 1, y + 1);
						Iy +=
							L.at<double>(i + x, j + y) * Gy.at<int>(x + 1, y + 1);
						 	
					}

			//std::cout << Ix << " " << Iy << std::endl;


			IMat = (Mat_<double>(2, 2) << Ix * Ix, Ix * Iy, Ix * Iy, Iy* Iy);

			for (int x = -2; x < 3; x++)
				for (int y = -2; y < 3; y++)
					if (i + x >= 0 && i + x < L.rows && j + y >= 0 && j + y < L.cols)
					{
						M += IMat; //w(x,y) = 1 в окне и 0 за пределами
					}
			//std::cout << M << std::endl << std::endl;
			double det = (M.at<double>(0, 0) * M.at<double>(1, 1)) - (M.at<double>(1, 0) * M.at<double>(0, 1));
			double tr = M.at<double>(0, 0) + M.at<double>(1, 1);
			double R = det - k * pow(tr, 2);
			//double R =  det / tr; //firstner
			//if(i == 5 && j == 100)(
			std::cout << det << std::endl << tr << std::endl << std::endl;
			if (R > 0)
				std::cout << R << std::endl;
		}
	}
	L /= 100;
	return L;
}

Mat forstner_detector(Mat orig)
{
	Mat L = getLMatrix(orig) * 100;
	Mat res(L.rows, L.cols, L.type());
	res = 0;
	Mat Gx = (Mat_<int>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	Mat Gy = (Mat_<int>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);

	for (int i = 0; i < orig.rows; i++)
	{
		for (int j = 0; j < orig.cols; j++)
		{
			Mat M(2, 2, L.type());
			M = 0;
			Mat IMat(2, 2, L.type());
			double Ix = 0, Iy = 0; //средние по столбцу и строке (или наоборот?) хотя не важно		
			for (int x = -1; x < 2; x++)
				for (int y = -1; y < 2; y++)
					if (i + x >= 0 && i + x < L.rows && j + y >= 0 && j + y < L.cols)
					{
						Ix +=
							L.at<double>(i + x, j + y) * Gx.at<int>(x + 1, y + 1);
						Iy +=
							L.at<double>(i + x, j + y) * Gy.at<int>(x + 1, y + 1);

					}
			//std::cout << Ix << " " << Iy << std::endl;

			IMat = (Mat_<double>(2, 2) << Ix * Ix, Ix * Iy, Ix * Iy, Iy* Iy);

			for (int x = -2; x < 3; x++)
				for (int y = -2; y < 3; y++)
					if (i + x >= 0 && i + x < L.rows && j + y >= 0 && j + y < L.cols)
					{
						M += IMat; //w(x,y) = 1 в окне и 0 за пределами
					}
			//std::cout << M << std::endl << std::endl;
			double det = (M.at<double>(0, 0) * M.at<double>(1, 1)) - (M.at<double>(1, 0) * M.at<double>(0, 1));
			double tr = M.at<double>(0, 0) + M.at<double>(1, 1);
			double R =  det / tr; 
			//std::cout << det << std::endl << tr << std::endl << std::endl; //1e-20
			if (R > 1e-10)
				res.at<double>(i, j) = 1;
				//std::cout << R << std::endl;
		}
	}
	L /= 100;
	return res;



}