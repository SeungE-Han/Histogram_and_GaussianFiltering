#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

float calc_Histo(const Mat& image, Mat& hist, int bins, int range_max = 256) { //������׷� ��� �Լ�
	int histSize[] = { bins }; //��Ⱚ ����(0~255)
	float range[] = { 0, (float)range_max }; //������׷� ȭ�Ұ� ����(0~255)
	int channels[] = { 0 }; //ä�� ���- ���� ä��
	const float* ranges[] = { range }; //��� ä�� ȭ�� ����
	
	calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges); //������׷� ���

	//for (int i = 0; i < hist.rows; i++)
	//	cout << "hist[" << i << "]" << hist.at<float>(i) << endl;

	float thr; //����ġ ���� ������ ����
	float bef, aft;	//bef: ����ġ�� �� ���� aft: ����ġ �� �� ����

	for (int i = 3; i<hist.rows - 3; i++) { //
		bef = hist.at<float>(i) - hist.at<float>(i - 3); //����ġ�� �� ����(3ĭ �� ���)
		aft = hist.at<float>(i + 3) - hist.at<float>(i); ////����ġ�� �� ����(3ĭ �� ���)

		if (bef < 0 && aft>0) //����ġ�� �� ���Ⱑ ������, ����ġ�� �� ���Ⱑ �����
			thr = i; //����ġ�� ����
	}
	
	return thr; //����ġ�� ����
}


void draw_histo(Mat hist, Mat& hist_img, Size size = Size(256, 200)) { //������׷� �׸��� �Լ�
	hist_img = Mat(size, CV_8U, Scalar(255)); //�Ͼ��
	float bin = (float)hist_img.cols / hist.rows; //�� ��� �ʺ�
	normalize(hist, hist, 0, hist_img.rows, NORM_MINMAX); //������׷��� �ִ��� ������ �ִ�����

	for (int i = 0; i < hist.rows; i++) {
		float start_x = i + bin; //���� �簢�� ���� x ��ǥ
		float end_x = (i + 1)* bin; //���� �簢�� ���� x ��ǥ
		Point2f pt1(start_x, 0);
		Point2f pt2(end_x, hist.at<float>(i)); //y������ ������ ��ŭ �׸�

		if (pt2.y > 0)
			rectangle(hist_img, pt1, pt2, Scalar(0), -1); //���������� ���� �簢�� �׸���
	}
	flip(hist_img, hist_img, 0); //x�� ���� ������
}

int main() {
	Mat img = imread("cellimg2.jpg", IMREAD_GRAYSCALE);
	CV_Assert(!img.empty()); //����ó��
	Mat img2 = Mat(size(img), CV_8UC1, Scalar(0)); //����ä�� ���� ���
	Mat img3 = Mat(size(img), CV_8UC1, Scalar(0)); //����ä�� ���� ���
	Mat img4 = Mat(size(img), CV_8UC1, Scalar(0)); //����ä�� ���� ���
	Point pt(0, 0);

	
	Mat hist2, hist2_img;
	float th1 = calc_Histo(img, hist2, 256);//������׷� ���
	draw_histo(hist2, hist2_img); //������׷� �׸���

	cout << "th1=" << th1 << endl;
	
	for (pt.y = 0; pt.y < img2.rows; pt.y++) {
		for (pt.x = 0; pt.x < img2.cols; pt.x++) {
			if (img.at<uchar>(pt.y, pt.x) > th1) //img�� ��Ⱚ�� ����ġ ������ ũ��
				img2.at<uchar>(pt.y, pt.x) = 255; //255�� ����
			else //�ƴϸ�
				img2.at<uchar>(pt.y, pt.x) = 0; //0���� ����
		}
	}
	
	Size size1(5, 5); //5X5 ����þ� ����ũ
	double sigMax1 = 0.3*((size1.width - 1)*0.5 - 1) + 0.8; //���ι��� ǥ������
	double sigMay1 = 0.3*((size1.height - 1)*0.5 - 1) + 0.8; //���ι��� ǥ������

	GaussianBlur(img, img3, size1, sigMax1, sigMay1); //openCV���� ����þ� ������ �Լ�
	
	Mat hist3, hist3_img;
	float th2 = calc_Histo(img3, hist3, 256); //������׷� ���
	draw_histo(hist3, hist3_img); //������׷� �׸���

	cout << "th2=" << th2 << endl;
	
	for (pt.y = 0; pt.y < img3.rows; pt.y++) {
		for (pt.x = 0; pt.x < img3.cols; pt.x++) {
			if (img.at<uchar>(pt.y, pt.x) > th2) //img�� ��Ⱚ�� ����ġ ������ ũ��
				img3.at<uchar>(pt.y, pt.x) = 255; //255�� ����
			else //�ƴϸ�
				img3.at<uchar>(pt.y, pt.x) = 0; //0���� ����
		}
	}
	
	
	Size size2(13, 13); //13X13 ����þ� ����ũ
	double sigMax2 = 0.3*((size2.width - 1)*0.5 - 1) + 0.8; //���ι��� ǥ������
	double sigMay2 = 0.3*((size2.height - 1)*0.5 - 1) + 0.8; //���ι��� ǥ������

	GaussianBlur(img, img4, size2, sigMax2, sigMay2); //openCV���� ����þ� ������ �Լ�
	
	Mat hist4, hist4_img;
	float th3 = calc_Histo(img4, hist4, 256); //������׷� ���
	draw_histo(hist4, hist4_img); //������׷� �׸���

	cout << "th3=" << th3 << endl;
	
	for (pt.y = 0; pt.y < img4.rows; pt.y++) {
		for (pt.x = 0; pt.x < img4.cols; pt.x++) {
			if (img.at<uchar>(pt.y, pt.x) > th3) //img�� ��Ⱚ�� ����ġ ������ ũ��
				img4.at<uchar>(pt.y, pt.x) = 255; //255�� ����
			else //�ƴϸ�
				img4.at<uchar>(pt.y, pt.x) = 0; //0���� ����
		}
	}

	imshow("����", img);
	imshow("hist2_img", hist2_img);
	imshow("img2", img2);
	imshow("hist3_img", hist3_img);
	imshow("img3", img3);
	imshow("hist4_img", hist4_img);
	imshow("img4", img4);
	waitKey(0);
	
	return 0;
}