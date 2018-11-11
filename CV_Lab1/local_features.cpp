#include "local_features.h"
#include "filtration.h"

Mat harris_detector(Mat orig)
{
	Mat L = getLMatrix(orig);
	/*Mat res(L.rows, L.cols, L.type());
	res = L;*/
	Mat res(L.rows, L.cols, L.type());
	L.copyTo(res);
	double k = 0.05; //k  [0.04, 0.06] //получать аргументом


	for (int i = 0; i < orig.rows; i++)
	{ 
		for (int j = 0; j < orig.cols; j++)
		{
			Mat M(2, 2, L.type());
			M = 0;
			Mat IMat(2, 2, L.type());
			
			for (int x = -2; x < 3; x++)
				for (int y = -2; y < 3; y++)
					if (i + x >= 2 && i + x < L.rows - 2 && j + y >= 2 && j + y < L.cols - 2)
					{
						double Ix = 0, Iy = 0;
						for (int x1 = -1; x1 < 2; x1++)
							for (int y1 = -1; y1 < 2; y1++)
							{
								
									Ix +=
										L.at<double>(i + x + x1, j + y + y1) * Gx.at<int>(x1 + 1, y1 + 1);
									Iy +=
										L.at<double>(i + x + x1, j + y + y1) * Gy.at<int>(x1 + 1, y1 + 1);
								
							}
						IMat = (Mat_<double>(2, 2) << Ix * Ix, Ix * Iy, Ix * Iy, Iy* Iy);
						M += IMat; //w(x,y) = 1 в окне и 0 за пределами //окно 3x3 в окне 5x5 :)
					}
					

			//std::cout << M << std::endl << std::endl;
			double det = (M.at<double>(0, 0) * M.at<double>(1, 1)) - (M.at<double>(1, 0) * M.at<double>(0, 1));
			double tr = M.at<double>(0, 0) + M.at<double>(1, 1);
			double R = det - k * pow(tr, 2); 
			/*if (i == 0 && j == 0)
				std::cout << M << std::endl;*/
			if (R > 0.25)
				//res.at<double>(i, j) = 1;
				res.at<double>(i, j) = 0;
		}
	}
	//L /= 100;
	return res;
}

Mat forstner_detector(Mat orig)
{
	Mat L = getLMatrix(orig) * 100;
	Mat res(L.rows, L.cols, L.type());
	res = 0;
	

	for (int i = 0; i < orig.rows; i++)
	{
		for (int j = 0; j < orig.cols; j++)
		{
			Mat M(2, 2, L.type());
			M = 0;
			Mat IMat(2, 2, L.type());
			double Ix = 0, Iy = 0; //средние по столбцу и строке (или наоборот?) хотя не важно		
			for (int x = -2; x < 3; x++)
				for (int y = -2; y < 3; y++)
					if (i + x >= 2 && i + x < L.rows - 2 && j + y >= 2 && j + y < L.cols - 2)
					{
						double Ix = 0, Iy = 0;
						for (int x1 = -1; x1 < 2; x1++)
							for (int y1 = -1; y1 < 2; y1++)
							{

								Ix +=
									L.at<double>(i + x + x1, j + y + y1) * Gx.at<int>(x1 + 1, y1 + 1);
								Iy +=
									L.at<double>(i + x + x1, j + y + y1) * Gy.at<int>(x1 + 1, y1 + 1);

							}
						IMat = (Mat_<double>(2, 2) << Ix * Ix, Ix * Iy, Ix * Iy, Iy* Iy);
						M += IMat; //w(x,y) = 1 в окне и 0 за пределами //окно 3x3 в окне 5x5 :)
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

void local_features(Mat orig)
{
	Mat L = getLMatrix(orig);
	Mat harris(L.rows, L.cols, L.type());
	Mat forstner(L.rows, L.cols, L.type());
	L.copyTo(harris);
	L.copyTo(forstner);

	double k = 0.05; //k  [0.04, 0.06] //получать аргументом

	for (int i = 0; i < orig.rows; i++)
	{
		for (int j = 0; j < orig.cols; j++)
		{
			Mat M(2, 2, L.type());
			M = 0;
			Mat IMat(2, 2, L.type());

			for (int x = -2; x < 3; x++)
				for (int y = -2; y < 3; y++)
					if (i + x >= 2 && i + x < L.rows - 2 && j + y >= 2 && j + y < L.cols - 2)
					{
						double Ix = 0, Iy = 0;
						for (int x1 = -1; x1 < 2; x1++)
							for (int y1 = -1; y1 < 2; y1++)
							{
								Ix +=
									L.at<double>(i + x + x1, j + y + y1) * Gx.at<int>(x1 + 1, y1 + 1);
								Iy +=
									L.at<double>(i + x + x1, j + y + y1) * Gy.at<int>(x1 + 1, y1 + 1);
							}
						IMat = (Mat_<double>(2, 2) << Ix * Ix, Ix * Iy, Ix * Iy, Iy* Iy);
						M += IMat; //w(x,y) = 1 в окне и 0 за пределами //окно 3x3 в окне 5x5 :)
					}

			double det = (M.at<double>(0, 0) * M.at<double>(1, 1)) - (M.at<double>(1, 0) * M.at<double>(0, 1));
			double tr = M.at<double>(0, 0) + M.at<double>(1, 1);
			double R;
			R = det - k * pow(tr, 2);
			if (R > 0.5)
				harris.at<double>(i, j) = 0;
			R = det / tr;
			if (R > 0.5)
				forstner.at<double>(i, j) = 0;

		}
	}
	imshow("Harris", harris);
	imshow("Forstner", forstner);

}


Mat SIFT(Mat orig, double sigma)
{
	
	Mat res1, res2, res;
	res1 = gauss_filter(orig, sigma);
	res2 = gauss_filter(orig, sigma*2);
	//std::cout << res2 - res1;
	res = res1 - res2;
	return res;
}