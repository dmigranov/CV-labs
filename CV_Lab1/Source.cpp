#include <opencv2/opencv.hpp>

using namespace cv;

Mat img;

void mouseCallback(int event, int x, int y, int flags, void * userdata)
{
	/*if (event == EVENT_LBUTTONDOWN)
	{
		int xy[2] = { x, y };
		userdata = xy;
	}*/
	if (event == EVENT_LBUTTONDOWN)
	{
		std::cout << x << " " << y << std::endl;
		Vec3b bgr = img.at<Vec3b>(y, x);
		uint blue = bgr[0];
		uint green = bgr[1];
		uint red = bgr[2];

		std::cout << "RGB: " << red << " " << green << " " << blue << std::endl;

		double mx = max(max(red, blue), green);
		double mn = min(min(red, blue), green);

		double value = mx / 256 * 100;
		double saturation = (1 - mn / mx)*100;
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



int main(int argc, char **argv)
{
	const char * wName = "Lab1";

	const char * imgname = (argc >= 2 ? argv[1] : "image.jpg");

	img = imread(imgname, CV_LOAD_IMAGE_COLOR);
	namedWindow(wName, WINDOW_AUTOSIZE);

	imshow(wName, img);

	setMouseCallback(wName, mouseCallback, NULL);


	waitKey(0);



	return 0;

}