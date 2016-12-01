#include"opencv2/opencv.hpp"
using namespace cv;

void buildGaussianPyramid(const Mat& base, vector<Mat>& pyr, int nOctaves);
void buildDoGPyramid(const vector<Mat>& gpyr, vector<Mat>& dogpyr);

int nOctaveLayers{ 3 };
double sigma{ 1.6 };

int main()
{
	Mat img = imread("ori.jpg");
	vector<Mat> gpyr,dogpyr;
	buildGaussianPyramid(img, gpyr, 2);
	buildDoGPyramid(gpyr,dogpyr);
	namedWindow("img", CV_WINDOW_AUTOSIZE);
	/* in this case gaussian pyramid has 12 layers,dog pyramid has 10 layers.
	For example,to access first layer DOG image,just use dogpyr[0] as follows */
	Mat img2 = dogpyr[0];
	imshow("img",img2);
	/**/
	waitKey(0);
	return 0;
}

// ����nOctaves�飨ÿ��nOctaveLayers+3�㣩��˹������  
void buildGaussianPyramid(const Mat& base, vector<Mat>& pyr, int nOctaves)
{
	vector<double> sig(nOctaveLayers + 3);
	pyr.resize(nOctaves*(nOctaveLayers + 3));

	// precompute Gaussian sigmas using the following formula:  
	//  \sigma_{total}^2 = \sigma_{i}^2 + \sigma_{i-1}^2��  
	// �����ͼ������ͬ�߶ȸ�˹ģ���ĳ߶�����  
	sig[0] = sigma;
	double k = pow(2., 1. / nOctaveLayers);
	for (int i = 1; i < nOctaveLayers + 3; i++)
	{
		double sig_prev = pow(k, (double)(i - 1))*sigma;
		double sig_total = sig_prev*k;
		sig[i] = std::sqrt(sig_total*sig_total - sig_prev*sig_prev);
	}

	for (int o = 0; o < nOctaves; o++)
	{
		// DoG��������ҪnOctaveLayers+2��ͼ�������nOctaves��߶�  
		// ���Ը�˹��������ҪnOctaveLayers+3��ͼ��õ�nOctaveLayers+2��DoG������  
		for (int i = 0; i < nOctaveLayers + 3; i++)
		{
			// dstΪ��o�飨Octave��������  
			Mat& dst = pyr[o*(nOctaveLayers + 3) + i];
			// ��0���0��Ϊԭʼͼ��  
			if (o == 0 && i == 0)
				dst = base;

			// base of new octave is halved image from end of previous octave  
			// ÿһ���0��ͼ��ʱ��һ�鵹��������ͼ���������õ�  
			else if (i == 0)
			{
				const Mat& src = pyr[(o - 1)*(nOctaveLayers + 3) + nOctaveLayers];
				resize(src, dst, Size(src.cols / 2, src.rows / 2),
					0, 0, INTER_NEAREST);
			}
			// ÿһ���i��ͼ�����ɵ�i-1��ͼ�����sig[i]�ĸ�˹ģ���õ�  
			// Ҳ���Ǳ���ͼ����sig[i]�ĳ߶ȿռ��µ�ͼ��  
			else
			{
				const Mat& src = pyr[o*(nOctaveLayers + 3) + i - 1];
				GaussianBlur(src, dst, Size(), sig[i], sig[i]);
			}
		}
	}
}

//����DOG������
void buildDoGPyramid(const vector<Mat>& gpyr, vector<Mat>& dogpyr)
{
	int nOctaves = (int)gpyr.size() / (nOctaveLayers + 3);
	dogpyr.resize(nOctaves*(nOctaveLayers + 2));

	for (int o = 0; o < nOctaves; o++)
	{
		for (int i = 0; i < nOctaveLayers + 2; i++)
		{
			// ��o���i��ͼ��Ϊ��˹�������е�o���i+1��i��ͼ������õ�  
			const Mat& src1 = gpyr[o*(nOctaveLayers + 3) + i];
			const Mat& src2 = gpyr[o*(nOctaveLayers + 3) + i + 1];
			Mat& dst_abs = dogpyr[o*(nOctaveLayers + 2) + i];
			Mat dst;
			subtract(src2, src1, dst, noArray(), CV_16S);
			convertScaleAbs(dst,dst_abs);//�����ת������ֵ��������ʾ
			
		}
	}
}