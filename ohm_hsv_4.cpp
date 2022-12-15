#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace cv::ml;
using namespace std;
int ohm[] = { 56,68,75,82,91,100,330,470 };
int i_cnt = 4;
void on_mouse(int event, int x, int y, int flags, void* userdata);
int iCnt = 8; //23
int main(void) {
	string str = "aohm-" + to_string(iCnt) + ".jpg";
	Mat src = imread(str, IMREAD_COLOR);
	resize(src, src, Size(4 * src.cols, 4 * src.rows));
	if (src.empty()) {
		cerr << "에러" << endl;
		return -1;
	}
	Mat src_blur;
	blur(src, src_blur, Size(7, 7));
//	namedWindow("src_blur");
//	setMouseCallback("src_blur", on_mouse, (void*)&src_blur);
	namedWindow("src");
	setMouseCallback("src", on_mouse, (void*)&src);
//	imshow("src_blur", src);
	imshow("src", src);
	// 평균값 필터?
	/*
	Mat src_hsv;
	cvtColor(src, src_hsv, COLOR_BGR2HSV);
	Mat src_hsv_blur;
	blur(src_hsv, src_hsv_blur, Size(7, 7));
	imshow("src_hsv_blur", src_hsv_blur);
	*/
	
	int k = waitKey();
}

Point ptOld;
void on_mouse(int event, int x, int y, int flags, void* userdata) {
	Mat src = *(Mat*)userdata;
	Mat rect;
	
	vector<int> hsv_vector_h, hsv_vector_s, hsv_vector_v;
	for (int i = 0; i < 255; i++) {
		hsv_vector_h.push_back(0);
		hsv_vector_s.push_back(0);
		hsv_vector_v.push_back(0);
	}

	switch (event) {
	case EVENT_LBUTTONDOWN:
		rect = Mat::zeros(1, 1, CV_8UC3);
		imshow("rect", rect);
		ptOld = Point(x, y);
		break;
	case EVENT_LBUTTONUP:
		rect = src(Rect(ptOld, Point(x, y))).clone();
		Mat rect_hsv;
		imshow("rect", rect);
		cvtColor(rect, rect_hsv, COLOR_BGR2HSV);

		int sum_h = 0, sum_s = 0, sum_v = 0;
		cout << "h s v = " << rect_hsv.at<Vec3b>(0, 0) << endl;
		for (int x = 0; x < rect_hsv.cols; x++) {
			for (int y = 0; y < rect_hsv.rows; y++) {
				Vec3b& p = rect_hsv.at<Vec3b>(y, x);
				sum_h += p[0];	sum_s += p[1];	sum_v += p[2];

				hsv_vector_h[p[0]]++;
				hsv_vector_s[p[1]]++;
				hsv_vector_v[p[2]]++;
			}
		}
		int size = (rect_hsv.cols - 1) * (rect_hsv.rows - 1);
		float avg_h = (float)sum_h / size;
		float avg_s = (float)sum_s / size;
		float avg_v = (float)sum_v / size;		
		cout << "평균 h,s,v = " << cvRound(avg_h) << " " << cvRound(avg_s) << " " << cvRound(avg_v) << endl;
		
		cout << "h-------------------------" << endl << endl;
		for (int i = 0; i < 255; i++) {
			if (hsv_vector_h[i] == 0) continue;
			cout << "h값이 " << i << "인 누적 픽셀 수 : " << hsv_vector_h[i] << endl;
		}

		cout << "s-------------------------" << endl << endl;
		for (int i = 0; i < 255; i++) {
			if (hsv_vector_s[i] == 0) continue;
			cout << "s값이 " << i << "인 누적 픽셀 수 : " << hsv_vector_s[i] << endl;
		}

		cout << "v-------------------------" << endl << endl;
		for (int i = 0; i < 255; i++) {
			if (hsv_vector_v[i] == 0) continue;
			cout << "v값이 " << i << "인 누적 픽셀 수 : " << hsv_vector_v[i] << endl;
		}


		cout << endl;
		break;
	}
}