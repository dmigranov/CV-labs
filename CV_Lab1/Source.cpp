#include <opencv2/opencv.hpp>

#define Xn 95.04
#define Yn 100.0
#define Zn 108.88

using namespace cv;

Mat img, r, g, b;

int hueSlider = 180;
int saturationSlider = 50;
int valueSlider = 50;

Mat h;
Mat s;
Mat v;


const char * wName = "Lab1";


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
		rgb /= 255; //rgb *= 100; //right??? //???????????
		std::cout << xyzmatrix << std::endl;
		Mat xyz = xyzmatrix * Mat(rgb);

		//std::cout << xyz << std::endl;
		//std::cout << xyz.at<double>(2, 0) << std::endl;
		double x = xyz.at<double>(0, 0);
		double y = xyz.at<double>(1, 0);
		double z = xyz.at<double>(2, 0);
		//LAB
		double L = 116 * labfunction(y / Yn) - 16; //rgb[1] = y
		double a = 500 * (labfunction(x / Xn) - labfunction(y / Yn));
		double b = 200 * (labfunction(y / Yn) - labfunction(z / Zn));
		std::cout << "L*a*b*: " << L << " " << a << " " << b << std::endl;
	}
	
}

void hueFunction(int, void *)
{
	
	//cvtColor(img, hsvimg, COLOR_BGR2HSV);
	//hsvimg += Vec3d(0, 0, 0);
	
}

void saturationFunction(int, void *)
{

}

void valueFunction(int, void *)
{
	
	

	Mat newV = v + Vec3d(0, 50 - valueSlider, 0);
	//value % 100;
	std::vector<Mat> hsv(h, s, v);
	Mat hsvimg;
	//merge(bgr, hsvimg);
	imshow(wName, hsvimg);
}

//restore original picture

int main(int argc, char **argv)
{
	

	const char * imgname = (argc >= 2 ? argv[1] : "image.jpg");

	img = imread(imgname, CV_LOAD_IMAGE_COLOR);
	Mat img_original = img.clone();
	namedWindow(wName, WINDOW_AUTOSIZE);


	//namedWindow("HSV", WINDOW_AUTOSIZE);

	createTrackbar("Hue", wName, &hueSlider, 360, hueFunction);
	createTrackbar("Saturation", wName, &saturationSlider, 100, saturationFunction);
	createTrackbar("Value", wName, &valueSlider, 100, valueFunction);
	//restore original picture button?
	std::vector<Mat> bgr;
	split(img, bgr);
	Mat r = bgr[2];
	Mat b = bgr[0];
	Mat g = bgr[1];

	Mat mx, mn;

	cv::max(r, b, mx);
	cv::max(mx, g, mx);

	cv::min(r, b, mn);
	cv::min(mx, g, mn);
	v = mx / 255 * 100;
	s = (1 - mn / mx) * 100;
	//h;





	imshow(wName, img);

	setMouseCallback(wName, mouseCallback, NULL);


	waitKey(0);

	return 0;

}