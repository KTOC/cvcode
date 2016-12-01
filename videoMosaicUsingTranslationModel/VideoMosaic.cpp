#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<iostream>

using namespace cv;
using namespace std;

Mat cylinder(Mat imgIn, int f);
Point2i getOffset(Mat img, Mat img1);
Mat linerFusion(Mat img, Mat img1, Point2i a);

int main()
{
	VideoCapture cap1(0);
	VideoCapture cap2(1);

	double rate = 60;
	int delay = 1000 / rate;
	bool stop(false);
	Mat frame1;
	Mat frame2;
	Mat frame;
	Point2i a;//�洢ƫ����
	int k = 0;

	namedWindow("cam1", CV_WINDOW_AUTOSIZE);
	namedWindow("cam2", CV_WINDOW_AUTOSIZE);
	namedWindow("stitch", CV_WINDOW_AUTOSIZE);

	if (cap1.isOpened()&&cap2.isOpened())
	{
		cout << "*** ***" << endl;
		cout << "����ͷ��������"<<endl;
	}
	else
	{
		cout << "*** ***" << endl;
		cout << "���棺����ͷ�򿪲��ɹ�����δ��⵽����������ͷ!" << endl;
		cout << "���������" <<endl<< "*** ***" << endl;
		return -1;
	}
	
	//cap1.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//cap1.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	//cap2.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//cap2.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cap1.set(CV_CAP_PROP_FOCUS,0);
	cap2.set(CV_CAP_PROP_FOCUS, 0);

	while (!stop)
	{
		if (cap1.read(frame1) && cap2.read(frame2))
		{
			imshow("cam1", frame1);
			imshow("cam2", frame2);

			//��ɫ֡ת�Ҷ�
			cvtColor(frame1, frame1, CV_RGB2GRAY);
			cvtColor(frame2, frame2, CV_RGB2GRAY);

			//����ͶӰ�任
			//frame1 = cylinder(frame1, 1005);
			//frame2 = cylinder(frame2, 1005);
			//ƥ���ƴ��
			if (k < 3 || waitKey(delay) == 13)
			{
				cout << "����ƥ��..."<<endl;
				a = getOffset(frame1, frame2);
			}
			frame = linerFusion(frame1, frame2, a);

			imshow("stitch", frame);
			k++;

		}
		else
		{
			cout << "----------------------" << endl;
			cout << "waitting..." << endl;
		}
			
		/*else
		{
			cout << "֡1��ȡʧ��" << endl;
			break;
		}

		if (cap2.read(frame2))
		{
			imshow("cam2", frame2);
		}
		else
		{
			cout << "֡2��ȡʧ��" << endl;
			break;
		}
		//��ɫ֡ת�Ҷ�
		cvtColor(frame1, frame1, CV_RGB2GRAY);
		cvtColor(frame2, frame2, CV_RGB2GRAY);

		//����ͶӰ�任
		//frame1 = cylinder(frame1, 1005);
		//frame2 = cylinder(frame2, 1005);
		//ƥ���ƴ��
		if (k < 3 || waitKey(delay) == 13)
		{

			a = getOffset(frame1, frame2);
		}
		frame = gradientStitch(frame1, frame2, a);

		imshow("fusion", frame);
		k++;*/
		if (waitKey(1) == 27)
		{
			stop = true;
			cout << "���������" << endl;
			cout << "*** ***" << endl;
		}
	}
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

Mat linerFusion(Mat img, Mat img1, Point2i a)
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

//����ͶӰУ��
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