#include "local_features.h"


Mat harris_detector(Mat orig)
{
	Mat L = getLMatrix(orig);
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
			int count = 0;
			/*for (int x = -2; x < 3; x++)
			{
				if (i + x >= 0 && i + x < L.rows)
				{ 
					Ix += L.at<double>(i + x, j);
					count++;
				}
			}
			Ix /= count;
			count = 0;
				
			for (int y = -2; y < 3; y++)
			{
				if (j + y >= 0 && j + y < L.cols)
				{
					Iy += L.at<double>(i, j + y);
					count++;
				}

			}
			Iy /= count;*/
			

			for (int x = -1; x < 2; x++)
				for (int y = -1; y < 2; y++)
					if (i + x >= 0 && i + x < L.rows && j + y >= 0 && j + y < L.cols)
					{
						Ix +=
							L.at<double>(i + x, j + y) * Gx.at<int>(x + 1, y + 1);
						Iy +=
							L.at<double>(i + x, j + y) * Gy.at<int>(x + 1, y + 1);
						 	
					}

			IMat = (Mat_<double>(2, 2) << Ix * Ix, Ix* Iy, Ix* Iy, Iy* Iy);

			for (int x = -2; x < 3; x++)
				for (int y = -2; y < 3; y++)
					if (i + x >= 0 && i + x < L.rows && j + y >= 0 && j + y < L.cols)
					{
						M += IMat; //w(x,y) = 1 в окне и 0 за пределами
					}

			double R = (M.at<double>(0, 0) * M.at<double>(1, 1)) - (M.at<double>(1, 0) * M.at<double>(0, 1)) - k * pow(M.at<double>(0, 0) + M.at<double>(1, 1), 2);
			//if(R > 0)
				std::cout << M << std::endl;
		}
	}
	return L;
}