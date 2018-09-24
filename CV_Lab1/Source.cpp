#include <opencv2/opencv.hpp>

using namespace cv;

Mat img;
int hueSlider;

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

		double value = mx / 256 * 100;
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
		
	}
	
}

static void hueFunction(int, void *)
{

}

int main(int argc, char **argv)
{
	const char * wName = "Lab1";

	const char * imgname = (argc >= 2 ? argv[1] : "image.jpg");

	img = imread(imgname, CV_LOAD_IMAGE_COLOR);
	Mat img_original = img.clone();
	namedWindow(wName, WINDOW_AUTOSIZE);
	
	createTrackbar("Hue", wName, &hueSlider, 360, hueFunction);


	imshow(wName, img);

	setMouseCallback(wName, mouseCallback, NULL);


	waitKey(0);



	return 0;

}