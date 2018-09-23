#include <opencv2/opencv.hpp>

void mouseCallback(int event, int x, int y, int flag, void * param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
		std::cout << x << " " << y << std::endl;
	
}



int main(int argc, char **argv)
{
	IplImage* image = NULL;
	IplImage* src = NULL;

	/*cv::Mat img = cv::imread("image.jpg");
	cv::imshow("img", img);
	cv::waitKey(0);
	return 0;*/

	const char * filename = "image.jpg";
	//char * dd = "fd";
	src = cvLoadImage(filename, 1); //> 0 <=> RGB; ==0 <=> grayscale 
	image = cvCloneImage(src);

	cvNamedWindow("Lab1", CV_WINDOW_AUTOSIZE);
	cvShowImage("Lab1", image);

	cvSetMouseCallback("Lab1", mouseCallback);



	cvWaitKey(0);
	cvReleaseImage(&image);
	cvReleaseImage(&src);
	cvDestroyWindow("Lab1");

	return 0;

}