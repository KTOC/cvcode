#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<iostream>

using namespace cv;
using namespace std;

int main()
{
	VideoCapture cap1(0);
	VideoCapture cap2(1);

	double rate = 60;
	int delay = 1000 / rate;
	bool stop(false);
	Mat frame1;
	Mat frame2;

	namedWindow("cam1", CV_WINDOW_AUTOSIZE);
	namedWindow("cam2", CV_WINDOW_AUTOSIZE);

	if (cap1.isOpened() && cap2.isOpened())
	{
		cout << "*** ***" << endl;
		cout << "����ͷ��������" << endl;
	}
	else
	{
		cout << "*** ***" << endl;
		cout << "���棺��������ͷ�Ƿ�װ��!" << endl;
		cout << "���������" << endl << "*** ***" << endl;
		return -1;
	}

	//cap1.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//cap1.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	//cap2.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//cap2.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cap1.set(CV_CAP_PROP_FOCUS, 0);
	cap2.set(CV_CAP_PROP_FOCUS, 0);

	while (!stop)
	{
		if (cap1.read(frame1) && cap2.read(frame2))
		{
			imshow("cam1", frame1);
			imshow("cam2", frame2);
			imwrite("frame1.bmp", frame1);
			imwrite("frame2.bmp", frame2);
			//��ɫ֡ת�Ҷ�
			cvtColor(frame1, frame1, CV_RGB2GRAY);
			cvtColor(frame2, frame2, CV_RGB2GRAY);


			if (waitKey(1) == 27)
			{
				stop = true;
				cout << "���������" << endl;
				cout << "*** ***" << endl;
			}
		}
	}
	return 0;
}