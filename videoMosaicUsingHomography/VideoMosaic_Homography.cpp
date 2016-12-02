#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
# include "opencv2/features2d/features2d.hpp"
#include"opencv2/nonfree/nonfree.hpp"
#include"opencv2/calib3d/calib3d.hpp"
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
	Mat img1;
	Mat img2;
	Mat result;
	int d = 200;//���뽥���ںϿ��
	Mat homography;
	int k = 0;

	namedWindow("cam1", CV_WINDOW_AUTOSIZE);
	namedWindow("cam2", CV_WINDOW_AUTOSIZE);
	namedWindow("stitch", CV_WINDOW_AUTOSIZE);

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

	cap1.set(CV_CAP_PROP_FOCUS, 0);
	cap2.set(CV_CAP_PROP_FOCUS, 0);

	while (!stop)
	{
		if (cap1.read(img1) && cap2.read(img2))
		{
			imshow("cam1", img1);
			imshow("cam2", img2);

			//��ɫ֡ת�Ҷ�
			//cvtColor(img1, img1, CV_RGB2GRAY);
			//cvtColor(img2, img2, CV_RGB2GRAY);

			//���㵥Ӧ����
			if (k < 1 || waitKey(delay) == 13)
			{
				cout << "����ƥ��..." << endl;
				////////////////////////////////
				vector<KeyPoint> keypoints1, keypoints2;
				//��������
				//Ptr<FeatureDetector> detector = new ORB(120);
				Ptr<FeatureDetector> detector = new SIFT(80);
				detector->detect(img1, keypoints1);
				detector->detect(img2, keypoints2);
				//������������ȡ��
				Ptr<DescriptorExtractor> descriptor = detector;
				//��ȡ������
				Mat descriptors1, descriptors2;

				descriptor->compute(img1, keypoints1, descriptors1);
				descriptor->compute(img2, keypoints2, descriptors2);
				//����ƥ����
				BFMatcher matcher(NORM_L2, true);
				//ƥ��������
				vector<DMatch> matches;
				matcher.match(descriptors1, descriptors2, matches);

				vector<Point2f> selPoints1, selPoints2;
				vector<int> pointIndexes1, pointIndexes2;
				for (vector<DMatch>::const_iterator it = matches.begin(); it != matches.end(); ++it)
				{
					selPoints1.push_back(keypoints1.at(it->queryIdx).pt);
					selPoints2.push_back(keypoints2.at(it->trainIdx).pt);
				}

				vector<uchar> inliers(selPoints1.size(), 0);
				homography = findHomography(selPoints1, selPoints2, inliers, CV_FM_RANSAC, 1.0);

				//����RANSAC����ɸѡƥ��
				vector<DMatch> outMatches;
				vector<uchar>::const_iterator itIn = inliers.begin();
				vector<DMatch>::const_iterator itM = matches.begin();
				for (; itIn != inliers.end(); ++itIn, ++itM)
				{
					if (*itIn)
					{
						outMatches.push_back(*itM);
					}

				}
				k++;

				//����ƥ����
				//Mat matchImage;
				//drawMatches(img1, keypoints1, img2, keypoints2, outMatches, matchImage, 255, 255);
				//imshow("match", matchImage);
				///////////////////////////////////////////////////////////////////////
				
			}
			//ƴ��
			double t = getTickCount();
			warpPerspective(img1, result, homography, Size(2 * img1.cols-200, img1.rows));//�вü���200�ɵ�
			//Mat half(result, Rect(0, 0, img2.cols, img2.rows));
			//img2.copyTo(half);

			Mat half(result, Rect(0, 0, img2.cols - d, img2.rows));
			img2(Range::all(), Range(0, img2.cols - d)).copyTo(half);
			for (int i = 0; i < d; i++)
			{
				result.col(img2.cols - d + i) = (d - i) / (float)d*img2.col(img2.cols - d + i) + i / (float)d*result.col(img2.cols - d + i);
			}
			//Mat crop(result,Rect(0,0,result.cols,result.rows-100));//�вü�
			//Mat roi(result, Rect(img1.cols-50, 0, 100, img1.rows));
			//GaussianBlur(roi, roi, Size(5, 5), 0);//���У��첻ֹ��ֱ�����У�λ�ò���

			imshow("stitch", result);
			t = ((double)getTickCount() - t) / getTickFrequency();
			//cout << t << endl;

		}
		else
		{
			cout << "----------------------" << endl;
			cout << "waitting..." << endl;
		}

		if (waitKey(1) == 27)
		{
			stop = true;
			cout << "���������" << endl;
			cout << "*** ***" << endl;
		}
	}
	return 0;
}