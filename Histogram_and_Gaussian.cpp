#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

float calc_Histo(const Mat& image, Mat& hist, int bins, int range_max = 256) { //히스토그램 계산 함수
	int histSize[] = { bins }; //밝기값 범위(0~255)
	float range[] = { 0, (float)range_max }; //히스토그램 화소값 범위(0~255)
	int channels[] = { 0 }; //채널 목록- 단일 채널
	const float* ranges[] = { range }; //모든 채널 화소 범위
	
	calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges); //히스토그램 계산

	//for (int i = 0; i < hist.rows; i++)
	//	cout << "hist[" << i << "]" << hist.at<float>(i) << endl;

	float thr; //문턱치 값을 저장할 변수
	float bef, aft;	//bef: 문턱치값 전 기울기 aft: 문턱치 값 후 기울기

	for (int i = 3; i<hist.rows - 3; i++) { //
		bef = hist.at<float>(i) - hist.at<float>(i - 3); //문턱치값 전 기울기(3칸 띄어서 계산)
		aft = hist.at<float>(i + 3) - hist.at<float>(i); ////문턱치값 후 기울기(3칸 띄어서 계산)

		if (bef < 0 && aft>0) //문턱치값 전 기울기가 음수고, 문턱치값 후 기울기가 양수면
			thr = i; //문턱치값 저장
	}
	
	return thr; //문턱치값 리턴
}


void draw_histo(Mat hist, Mat& hist_img, Size size = Size(256, 200)) { //히스토그램 그리는 함수
	hist_img = Mat(size, CV_8U, Scalar(255)); //하얀색
	float bin = (float)hist_img.cols / hist.rows; //한 계급 너비
	normalize(hist, hist, 0, hist_img.rows, NORM_MINMAX); //히스토그램의 최댓값을 높이의 최댓값으로

	for (int i = 0; i < hist.rows; i++) {
		float start_x = i + bin; //막대 사각형 시작 x 좌표
		float end_x = (i + 1)* bin; //막대 사각형 종료 x 좌표
		Point2f pt1(start_x, 0);
		Point2f pt2(end_x, hist.at<float>(i)); //y축으로 누적된 만큼 그림

		if (pt2.y > 0)
			rectangle(hist_img, pt1, pt2, Scalar(0), -1); //검은색으로 막대 사각형 그리기
	}
	flip(hist_img, hist_img, 0); //x축 기준 뒤집기
}

int main() {
	Mat img = imread("cellimg2.jpg", IMREAD_GRAYSCALE);
	CV_Assert(!img.empty()); //예외처리
	Mat img2 = Mat(size(img), CV_8UC1, Scalar(0)); //단일채널 영상 출력
	Mat img3 = Mat(size(img), CV_8UC1, Scalar(0)); //단일채널 영상 출력
	Mat img4 = Mat(size(img), CV_8UC1, Scalar(0)); //단일채널 영상 출력
	Point pt(0, 0);

	
	Mat hist2, hist2_img;
	float th1 = calc_Histo(img, hist2, 256);//히스토그램 계산
	draw_histo(hist2, hist2_img); //히스토그램 그리기

	cout << "th1=" << th1 << endl;
	
	for (pt.y = 0; pt.y < img2.rows; pt.y++) {
		for (pt.x = 0; pt.x < img2.cols; pt.x++) {
			if (img.at<uchar>(pt.y, pt.x) > th1) //img의 밝기값이 문턱치 값보다 크면
				img2.at<uchar>(pt.y, pt.x) = 255; //255로 설정
			else //아니면
				img2.at<uchar>(pt.y, pt.x) = 0; //0으로 설정
		}
	}
	
	Size size1(5, 5); //5X5 가우시안 마스크
	double sigMax1 = 0.3*((size1.width - 1)*0.5 - 1) + 0.8; //가로방향 표준편차
	double sigMay1 = 0.3*((size1.height - 1)*0.5 - 1) + 0.8; //세로방향 표준편차

	GaussianBlur(img, img3, size1, sigMax1, sigMay1); //openCV제공 가우시안 스무딩 함수
	
	Mat hist3, hist3_img;
	float th2 = calc_Histo(img3, hist3, 256); //히스토그램 계산
	draw_histo(hist3, hist3_img); //히스토그램 그리기

	cout << "th2=" << th2 << endl;
	
	for (pt.y = 0; pt.y < img3.rows; pt.y++) {
		for (pt.x = 0; pt.x < img3.cols; pt.x++) {
			if (img.at<uchar>(pt.y, pt.x) > th2) //img의 밝기값이 문턱치 값보다 크면
				img3.at<uchar>(pt.y, pt.x) = 255; //255로 설정
			else //아니면
				img3.at<uchar>(pt.y, pt.x) = 0; //0으로 설정
		}
	}
	
	
	Size size2(13, 13); //13X13 가우시안 마스크
	double sigMax2 = 0.3*((size2.width - 1)*0.5 - 1) + 0.8; //가로방향 표준편차
	double sigMay2 = 0.3*((size2.height - 1)*0.5 - 1) + 0.8; //세로방향 표준편차

	GaussianBlur(img, img4, size2, sigMax2, sigMay2); //openCV제공 가우시안 스무딩 함수
	
	Mat hist4, hist4_img;
	float th3 = calc_Histo(img4, hist4, 256); //히스토그램 계산
	draw_histo(hist4, hist4_img); //히스토그램 그리기

	cout << "th3=" << th3 << endl;
	
	for (pt.y = 0; pt.y < img4.rows; pt.y++) {
		for (pt.x = 0; pt.x < img4.cols; pt.x++) {
			if (img.at<uchar>(pt.y, pt.x) > th3) //img의 밝기값이 문턱치 값보다 크면
				img4.at<uchar>(pt.y, pt.x) = 255; //255로 설정
			else //아니면
				img4.at<uchar>(pt.y, pt.x) = 0; //0으로 설정
		}
	}

	imshow("원본", img);
	imshow("hist2_img", hist2_img);
	imshow("img2", img2);
	imshow("hist3_img", hist3_img);
	imshow("img3", img3);
	imshow("hist4_img", hist4_img);
	imshow("img4", img4);
	waitKey(0);
	
	return 0;
}