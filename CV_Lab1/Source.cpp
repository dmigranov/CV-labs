#include <opencv2/opencv.hpp>

#define Xn 0.9504
#define Yn 1.0
#define Zn 1.0888 //если не умножать rgb/255 на 100
/*#define Xn 95.04
#define Yn 100.0
#define Zn 108.88*/


using namespace cv;

Mat img_original;
Mat img;

int hueSlider = 180, hsprev = 180;
int saturationSlider = 50, ssprev = 50;
int valueSlider = 50, vsprev = 50;



const char * wName = "Lab1";

void L_histogram()
{
	Mat labimg, hist;

	cvtColor(img, labimg, COLOR_BGR2Lab);
	std::vector<Mat> labv;
	split(labimg, labv);
	//int histSize = ;
	float range[] = {0, 100};
	const float * histRange = { range };
	int histSize = 100;
	int chanels[] = { 0 };

	//calcHist(&labimg, 1, chanels, Mat(), hist, 1, &histSize, &range, true, false);
	calcHist(&labv[0], 1, 0, Mat(), hist, 1, &histSize, &histRange, true, false);
	//ВЫВОД 



	imshow("Histogram", hist);
}

double labfunction(double x)
{
	if (x > pow(6.0 / 29, 3))
	{
		return pow(x, 1.0 / 3);
	}
	else
	{
		return pow(29.0 / 6, 2) / 3 + 4.0 / 29;
	}
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
		
		//XYZ

		Mat xyzmatrix = (Mat_<double>(3, 3) << 2.768892, 1.751748, 1.13016, 1.0, 4.5907, 0.0601, 0.0, 0.056508, 5.594292);
		Vec3d rgb(red, green, blue);
		rgb /= 255;
		Mat xyz = xyzmatrix * Mat(rgb);
		xyz /= 5.6508;
		//проверить правильность перевода в X Y Z!!!
		double x = xyz.at<double>(0, 0);
		double y = xyz.at<double>(1, 0);
		double z = xyz.at<double>(2, 0);
		std::cout << xyz << std::endl;
		//LAB
		double L = 116 * labfunction(y / Yn) - 16; 
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
	hsvplains[0] += hueSlider - hsprev;

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
	hsvplains[2] += 2 * (valueSlider - vsprev); //коэффициенты?

	merge(hsvplains, hsvimg);
	Mat newimg;
	cvtColor(hsvimg, img, COLOR_HSV2BGR);
	vsprev = valueSlider;
	imshow(wName, img);
	
}//проверить пределы

//restore original picture

void redraw()
{
	img = img_original.clone();
	hsprev = 180;
	ssprev = 50;
	vsprev = 50;
	setTrackbarPos("Hue", wName, 180);
	setTrackbarPos("Saturation", wName, 50);
	setTrackbarPos("Value", wName, 50);

	
	imshow(wName, img);
}

int main(int argc, char **argv)
{
	

	const char * imgname = (argc >= 2 ? argv[1] : "image.jpg");

	img = imread(imgname, CV_LOAD_IMAGE_COLOR);
	img_original = img.clone();
	namedWindow(wName, WINDOW_AUTOSIZE);


	//namedWindow("HSV", WINDOW_AUTOSIZE);

	createTrackbar("Hue", wName, &hueSlider, 360, hueFunction);
	createTrackbar("Saturation", wName, &saturationSlider, 100, saturationFunction);
	createTrackbar("Value", wName, &valueSlider, 100, valueFunction);
	
	

	imshow(wName, img);

	setMouseCallback(wName, mouseCallback, NULL);

	while(1)
	{
		int k = waitKey(20);
		if (k == 'r')
			redraw();
		else if (k == 'h')
			L_histogram();
		else if (k == '0')
			break;

	}

	return 0;

}