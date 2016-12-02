#include"opencv2/core/core.hpp"
#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include<iostream>
#include<thread>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<time.h>

using namespace std;
using namespace cv;

Mat cylinder(Mat imgIn, int f);
Point2i getOffset(Mat img, Mat img1);
Mat gradientStitch(Mat img, Mat img1, Point2i a);

mutex mut;
queue<Point2i> data_queue;
condition_variable data_cond;

VideoCapture cap1(0);
VideoCapture cap2(1);
Mat frame1;
Mat frame2;
Mat frame;
Point2i a;//�洢ƫ����
bool run = true;
double t = (double)getTickCount();//�߳�2ʱ��
double t1 = (double)getTickCount();//�߳�1ʱ��
void data_preparation_thread()
{
	while (run)
	{
		if (cap1.read(frame1) && cap2.read(frame2))
		{
			waitKey(1);
			if (waitKey(20) >= 0)
			{
				run = false;
			}
			imshow("cam1", frame1);
			imshow("cam2", frame2);

			//��ɫ֡ת�Ҷ�
			cvtColor(frame1, frame1, CV_RGB2GRAY);
			cvtColor(frame2, frame2, CV_RGB2GRAY);

			//ƥ��
			//cout << "����ƥ��..." << endl;
			t1 = ((double)getTickCount() - t1) / getTickFrequency();
			cout <<"ƥ��ʱ�䣺"<< t1 << endl;
			a = getOffset(frame1, frame2);
			t1 = (double)getTickCount();
		}
	
		lock_guard<mutex> lk(mut);
		data_queue.push(a);
		data_cond.notify_one();

	}
}

void data_processing_thread()
{
	while (run)
	{
		unique_lock<mutex> lk(mut);
		data_cond.wait(lk, []{return !data_queue.empty(); });
		Point2i a = data_queue.front();
		data_queue.pop();
		lk.unlock();
		//cout << "����ƴ��"<<endl;
		frame = gradientStitch(frame1, frame2, a);
		waitKey(20);
		if (waitKey(20) >= 0)
		{
			run = false;
		}
		t = ((double)getTickCount() - t) / getTickFrequency();
		cout <<"ƴ��ʱ�䣺"<< t << endl;
		imshow("stitch", frame);
		t = (double)getTickCount();

	}
}

int main()
{
	//��ʼ��
	if (cap1.isOpened() && cap2.isOpened())
	{
		cout << "*** ***" << endl;
		cout << "����ͷ��������" << endl;
	}
	else
	{
		cout << "*** ***" << endl;
		cout << "���棺��������ͷ������2���Ƿ�װ��!" << endl;
		cout << "���������" << endl << "*** ***" << endl;
		return -1;
	}
	cap1.set(CV_CAP_PROP_FOCUS, 0);
	cap2.set(CV_CAP_PROP_FOCUS, 0);
	
	//ƥ������߳�
	thread t1(data_preparation_thread);
	//ƴ���߳�
	thread t2(data_processing_thread);
	t1.join();
	t2.join();

	return 0;
}

Point2i getOffset(Mat img, Mat img1)
{
	Mat templ(img1, Rect(0, 0.4*img1.rows, 0.2*img1.cols, 0.2*img1.rows));
	Mat result(img.cols - templ.cols + 1, img.rows - templ.rows + 1, CV_8UC1);//result���ƥ��λ����Ϣ
	matchTemplate(img, templ, result, CV_TM_CCORR_NORMED);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	double minVal; double maxVal; Point minLoc; Point maxLoc; Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	matchLoc = maxLoc;//������ƥ��λ��
	int dx = matchLoc.x;
	int dy = matchLoc.y - 0.4*img1.rows;//��ͼ�������ͼ���λ��
	Point2i a(dx, dy);
	return a;
}


Mat gradientStitch(Mat img, Mat img1, Point2i a)
{
	int d = img.cols - a.x;//���������
	int ms = img.rows - abs(a.y);//ƴ��ͼ����
	int ns = img.cols + a.x;//ƴ��ͼ����
	Mat stitch = Mat::zeros(ms, ns, CV_8UC1);
	//ƴ��
	Mat_<uchar> ims(stitch);
	Mat_<uchar> im(img);
	Mat_<uchar> im1(img1);

	if (a.y >= 0)
	{
		Mat roi1(stitch, Rect(0, 0, a.x, ms));
		img(Range(a.y, img.rows), Range(0, a.x)).copyTo(roi1);
		Mat roi2(stitch, Rect(img.cols, 0, a.x, ms));
		img1(Range(0, ms), Range(d, img1.cols)).copyTo(roi2);
		for (int i = 0; i < ms; i++)
			for (int j = a.x; j < img.cols; j++)
				ims(i, j) = uchar((img.cols - j) / float(d)*im(i + a.y, j) + (j - a.x) / float(d)*im1(i, j - a.x));

	}
	else
	{
		Mat roi1(stitch, Rect(0, 0, a.x, ms));
		img(Range(0, ms), Range(0, a.x)).copyTo(roi1);
		Mat roi2(stitch, Rect(img.cols, 0, a.x, ms));
		img1(Range(-a.y, img.rows), Range(d, img1.cols)).copyTo(roi2);
		for (int i = 0; i < ms; i++)
			for (int j = a.x; j < img.cols; j++)
				ims(i, j) = uchar((img.cols - j) / float(d)*im(i, j) + (j - a.x) / float(d)*im1(i + abs(a.y), j - a.x));
	}


	return stitch;
}