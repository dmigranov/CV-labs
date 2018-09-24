#include <opencv2/opencv.hpp>

using namespace cv;

void mouseCallback(int event, int x, int y, int flags, void * userdata)
{
	if (event == CV_EVENT_LBUTTONDOWN)
		std::cout << x << " " << y << std::endl;
	
}



int main(int argc, char **argv)
{


	Mat img = imread("image.jpg");
	//imshow("img", img);

	setMouseCallback("Lab1", mouseCallback, NULL);

	waitKey(0);



	return 0;

}