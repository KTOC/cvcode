#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include<iostream>
#include<stdio.h>
#include<time.h>

using namespace std;
using namespace cv;

Mat cylinder(Mat imgIn, int f)
{
	int colNum, rowNum;
	colNum = 2 * f*atan(0.5*imgIn.cols / f);//����ͼ���
	rowNum = 0.5*imgIn.rows*f / sqrt(pow(f, 2)) + 0.5*imgIn.rows;//����ͼ���

	Mat imgOut = Mat::zeros(rowNum, colNum, CV_8UC1);
	Mat_<uchar> im1(imgIn);
	Mat_<uchar> im2(imgOut);

	//�����ֵ
	int x1(0), y1(0);
	for (int i = 0; i < imgIn.rows; i++)
		for (int j = 0; j < imgIn.cols; j++)
		{
			x1 = f*atan((j - 0.5*imgIn.cols) / f) + f*atan(0.5*imgIn.cols / f);
			y1 = f*(i - 0.5*imgIn.rows) / sqrt(pow(j - 0.5*imgIn.cols, 2) + pow(f, 2)) + 0.5*imgIn.rows;
			if (x1 >= 0 && x1 < colNum&&y1 >= 0 && y1<rowNum)
			{
				im2(y1, x1) = im1(i, j);
			}
		}
	return imgOut;
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
	Point2i a(dx,dy);
	return a;
}	


Mat gradientStitch(Mat img, Mat img1,Point2i a)
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

int main()
{
	Mat img = imread("frame1.jpg", 0);//��ͼ��
	Mat img1 = imread("frame2.jpg", 0);//��ͼ��
	double t = (double)getTickCount();
	//����ͶӰ
	double t3 = (double)getTickCount();
	//img = cylinder(img,1000);
	//img1 = cylinder(img1, 1000);
	t3 = ((double)getTickCount() - t3) / getTickFrequency();
	//ƥ��
	double t1 = (double)getTickCount();
	Point2i a = getOffset(img, img1);
	t1 = ((double)getTickCount() - t1) / getTickFrequency();
	//ƴ��
	double t2 = (double)getTickCount();
	Mat stitch = gradientStitch(img, img1, a);
	t2 = ((double)getTickCount() - t2) / getTickFrequency();
	t = ((double)getTickCount() - t) / getTickFrequency();

	cout << "ͶӰʱ�䣺"<<t3<<'\n'<<"ƥ��ʱ�䣺"<<t1<<'\n'<<"ƴ��ʱ�䣺"<<t2 << endl;
	cout << "��ʱ��" << t << endl;

	namedWindow("Image Left", CV_WINDOW_AUTOSIZE);
	namedWindow("Image Right", CV_WINDOW_AUTOSIZE);
	namedWindow("Result Window", CV_WINDOW_AUTOSIZE);
	imshow("Image Left", img);
	imshow("Image Right", img1);
	imshow("Result Window", stitch);
	waitKey(0);
	return 0;
}