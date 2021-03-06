#include "morphology.h"
#include "lab.h"


Mat invertion(Mat orig)
{
	Mat res(orig.rows, orig.cols, orig.type());
	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
			res.at<uchar>(i, j) = 255 - orig.at<uchar>(i, j);
	return res;
}

Mat dilation(Mat orig, Mat oper)
{

	Mat res(orig.rows, orig.cols, orig.type());
	res = 0;
	//std::cout << orig;
	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			if (orig.at<uchar>(i, j) == 255) //255 - ��� � ������� 8U, �.� uchar; � double ���� 1
				for (int x = -oper.rows / 2; x < oper.rows / 2 + 1; x++)
					for (int y = -oper.cols / 2; y < oper.cols / 2 + 1; y++)
					{
						if (i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols)
						{
							if(res.at<uchar>(i + x, j + y) == 0)
								res.at<uchar>(i + x, j + y) = oper.at<uchar>(oper.rows / 2 + x, oper.cols / 2 + y);
						}
					}
		}

	return res;
}



Mat erosion(Mat orig, Mat oper)
{
	Mat res(orig.rows, orig.cols, orig.type());
	res = 0;

	uint operNumber = 0;
	for (int x = 0; x < oper.rows; x++)
		for (int y = 0; y < oper.cols; y++)
			if (oper.at<uchar>(x, y) == 255)
				operNumber++;

	for (int i = 0; i < orig.rows; i++)
		for (int j = 0; j < orig.cols; j++)
		{
			if (orig.at<uchar>(i, j) == 255)
			{
				uint count = 0;
				for (int x = -oper.rows / 2; x < oper.rows / 2 + 1; x++)
					for (int y = -oper.cols / 2; y < oper.cols / 2 + 1; y++)
					{
						if ((i + x >= 0 && i + x < orig.rows && j + y >= 0 && j + y < orig.cols) && orig.at<uchar>(i + x, j + y) == 255 && oper.at<uchar>(oper.rows / 2 + x, oper.cols / 2 + y) == 255)
						{
							count++;
						}
					}
				
				//std::cout << count << std::endl;
				if (count == operNumber)
				{

					res.at<uchar>(i, j) = 255;
				}
			}
		}

	return res;
}

Mat closing(Mat orig, Mat oper)
{
	return erosion(dilation(orig, oper), oper);
}

Mat opening(Mat orig, Mat oper)
{
	return dilation(erosion(orig, oper), oper);
}

uint countObjects(Mat orig)
{
	
	uint count = 1;
	Mat labels(orig.rows, orig.cols, orig.type());
	labels = 0;
	/*uint all = orig.rows * orig.cols;
	uint ones = 0;
	for (int x = 0; x < orig.rows; x++)
		for (int y = 0; y < orig.cols; y++)
			if (orig.at<uchar>(x, y) == 255)
				ones++;*/

	for (int x = 0; x < orig.rows; x++)
		for (int y = 0; y < orig.cols; y++)
			fillWithMark(orig, labels, count++, x, y);

	std::set<uint> objects;
	for (int x = 0; x < labels.rows; x++)
		for (int y = 0; y < labels.cols; y++)
			objects.insert(labels.at<uchar>(x, y));
	objects.erase(0);

	for (uint o : objects)
	{
		Mat object(orig.rows, orig.cols, orig.type());
		object = 0;
		for (int x = 0; x < labels.rows; x++)
			for (int y = 0; y < labels.cols; y++)
				if (labels.at<uchar>(x, y) == o)
					object.at< uchar >(x, y) = 255;
		std::string wName = "Object " + std::to_string(o);
		imshow(wName, object);
	}

	return objects.size(); //������ ��� ���� ���� �������
}

void fillWithMark(Mat &orig, Mat &labels, uint label, uint x, uint y)
{
	
	if (labels.at<uchar>(x, y) == 0 && orig.at<uchar>(x, y) == 255)
	{
		//std::cout << label << std::endl;
		labels.at<uchar>(x, y) = label;
		if (x > 0)
			fillWithMark(orig, labels, label, x - 1, y);
		if (y > 0)
			fillWithMark(orig, labels, label, x, y - 1);
		if (x < orig.rows - 1)
			fillWithMark(orig, labels, label, x + 1, y);
		if (y < orig.cols - 1)
			fillWithMark(orig, labels, label, x, y + 1);
	}
}
