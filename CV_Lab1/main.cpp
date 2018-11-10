#include <opencv2/opencv.hpp>
#include "filtration.h"
#include "lab.h"
#include "morphology.h"
#include "segmentation.h"
#include "local_features.h"

using namespace cv;

Mat img_original;
Mat img;
Mat img_2;

int hueSlider = 180, hsprev = 180;
int saturationSlider = 100, ssprev = 100;
int valueSlider = 100, vsprev = 200;
int sigmaSlider = 0;

int lowerSlider = 10;
int upperSlider = 25;

int thetaSlider = 0;


const char * wName = "Lab1";
const char * wName_2 = "Lab2";

void L_histogram()
{
	Mat labimg, hist;

	cvtColor(img, labimg, COLOR_BGR2Lab);
	std::vector<Mat> labv;
	split(labimg, labv);
	//int histSize = ;
	float range[] = {0, 100};
	const float * histRange = { range };
	int histSize = 100; //number of bins
	int chanels[] = { 0 };

	//calcHist(&labimg, 1, chanels, Mat(), hist, 1, &histSize, &range, true, false);
	calcHist(&labv[0], 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
	//ВЫВОД 

	int hist_w = 200, hist_h = 400;
	int bin_w = hist_w / histSize;
	Mat histImg(hist_h, hist_w, CV_32FC1);
	normalize(hist, hist, 0, histImg.rows, NORM_MINMAX, -1, Mat());

	for (int i = 1; i < histSize; i++)
	{
		line(histImg, Point(bin_w*(i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(hist.at<float>(i))),
			Scalar(100, 0, 0), 2, 8, 0);
	}

	imshow("Histogram", histImg);
}


void mouseCallback(int event, int x, int y, int flags, void * userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		std::cout << x << " " << y << std::endl;
		Vec3b bgr = img.at<Vec3b>(y, x);
		double blue = bgr[0];
		double green = bgr[1];
		double red = bgr[2];

		std::cout << "RGB: " << red << " " << green << " " << blue << std::endl;

		double mx = max(max(red, blue), green);
		double mn = min(min(red, blue), green);

		double value = mx / 255 * 100;
		double saturation = mx != 0 ? (1 - mn / mx) * 100 : 0;
		double hue;

		if (mx == red)
		{
			hue = 60 * (green - blue) / (mx - mn);
			if (green < blue)
				hue += 360;
		}
		else if (mx == green)
			hue = 60 * (blue - red) / (mx - mn) + 120;
		else
			hue = 60 * (red - green) / (mx - mn) + 240;

		std::cout << "HSV: " << hue << " " << saturation << " " << value << std::endl;
		
		
		Vec3d xyz = getXYZ(red, green, blue);
		double x = xyz[0];
		double y = xyz[1];
		double z = xyz[2];
		std::cout << xyz << std::endl;
		//LAB
		double L = (116.0 * labfunction(y / Yn) - 16);
		double a_ast = 500 * (labfunction(x / Xn) - labfunction(y / Yn));
		double b_ast = 200 * (labfunction(y / Yn) - labfunction(z / Zn));
		std::cout << "L*a*b*: " << L << " " << a_ast << " " << b_ast << std::endl;
	}
}

void hueFunction(int, void *)
{
	Mat hsvimg;
	std::vector<Mat> hsvplains;
	
	cvtColor(img, hsvimg, COLOR_BGR2HSV);
	split(hsvimg, hsvplains);
	hsvplains[0] += (hueSlider - hsprev);

	merge(hsvplains, hsvimg);
	Mat newimg;
	cvtColor(hsvimg, img, COLOR_HSV2BGR);
	hsprev = hueSlider;
	imshow(wName, img);
} //проверить пределы

void saturationFunction(int, void *)
{
	Mat hsvimg;
	std::vector<Mat> hsvplains;

	cvtColor(img, hsvimg, COLOR_BGR2HSV);
	split(hsvimg, hsvplains);
	hsvplains[1] += 2*(saturationSlider - ssprev);

	merge(hsvplains, hsvimg);
	Mat newimg; //
	cvtColor(hsvimg, img, COLOR_HSV2BGR);
	ssprev = saturationSlider;
	imshow(wName, img);
}//проверить пределы

void valueFunction(int, void *)
{
	Mat hsvimg;
	std::vector<Mat> hsvplains;

	cvtColor(img, hsvimg, COLOR_BGR2HSV);
	split(hsvimg, hsvplains);
	hsvplains[2] +=  2*(valueSlider - vsprev); //коэффициенты?

	merge(hsvplains, hsvimg);
	Mat newimg;
	cvtColor(hsvimg, img, COLOR_HSV2BGR);
	vsprev = valueSlider;
	imshow(wName, img);
	
}//проверить пределы

void sigmaFunction(int, void *)
{

}

void ulFunction(int, void *)
{
	//imshow("Canny", canny(img, lowerSlider / 100.0, upperSlider / 100.0));
}

void gaborFunction(int, void *)
{
	//imshow("Gabor", gabor_filter(canny(img, 0.2, 0.25), thetaSlider * M_PI / 180, 0, 0.5, 0.2, 2));
}

void redraw()
{
	img = img_original.clone();
	img_2 = img.clone();
	hsprev = 180;
	ssprev = 100;
	vsprev = 100;
	setTrackbarPos("Hue", wName, 180);
	setTrackbarPos("Saturation", wName, 100);
	setTrackbarPos("Value", wName, 100);

	imshow(wName, img);
}

int main(int argc, char **argv)
{
	const char * imgname = (argc >= 2 ? argv[1] : "image.jpg");

	img = imread(imgname, CV_LOAD_IMAGE_COLOR);
	img_2 = img.clone();
	img_original = img.clone();
	namedWindow(wName, WINDOW_AUTOSIZE);
	createTrackbar("Hue", wName, &hueSlider, 360, hueFunction);
	createTrackbar("Saturation", wName, &saturationSlider, 200, saturationFunction);
	createTrackbar("Value", wName, &valueSlider, 200, valueFunction);
	createTrackbar("Sigma", wName, &sigmaSlider, 25, sigmaFunction);

	namedWindow(wName_2, WINDOW_AUTOSIZE);
	createTrackbar("Lower edge", wName_2, &lowerSlider, 100, ulFunction);
	createTrackbar("Upper edge", wName_2, &upperSlider, 100, ulFunction);
	createTrackbar("Theta", wName_2, &thetaSlider, 180, gaborFunction); //360?

	imshow(wName, img);

	setMouseCallback(wName, mouseCallback, NULL);

	while (1)
	{
		int k = waitKey(20);
		if (k == 'r')
			redraw();
		else if (k == 'h')
			L_histogram();
		else if (k == 'g')
		{
			if (sigmaSlider != 0)
			{
				img = gauss_filter(img, sigmaSlider);
				imshow(wName, img);
			}
		}
		else if (k == 'G')
		{
			imshow(wName_2, gabor_filter(img_2, thetaSlider * M_PI / 180, 0, 0.5, 0.2, 2));

		}
		else if (k == 's')
		{
			imshow("Sobel", sobel_filter(img, NULL));
		}
		else if (k == 'S')
		{
			//std::cout << "Mean: " << mean(getLMatrix(img)) << std::endl << "Homogeneity: " << homogeneity(getLMatrix(img)) << std::endl;
			
			imshow("Split", splitmerge(img));
		}
		else if (k == 'l')
		{
			imshow("My luminance", getLMatrix(img));
		}
		else if (k == 'L')
		{
			//imshow("Forstner", forstner_detector(img));
			imshow("Harris", harris_detector(img));
		}
		else if (k == 'c')
		{
			img_2 = canny(img, lowerSlider / 100.0, upperSlider / 100.0);
			imshow(wName_2, img_2);
		}
		else if (k == 'o')
		{
			imshow("Otsu", otsu(img));
		}
		else if (k == 'm')
		{
			Mat morphimg = invertion(otsu(img));
			Mat newimg = erosion(dilation(morphimg, circle5x5), circle7x7); //по сути closing с разными матрицами
			std::cout << "Cell count: " << countObjects(dilation(opening(newimg, circle3x3), circle3x3)) << std::endl; ///final
		}
		else if (k == '0')
			break;
		

	}

	return 0;

}