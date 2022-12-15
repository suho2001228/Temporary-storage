#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace cv::ml;
using namespace std;
/*
int ohm[] = { 56,68,75,82,91,100,330,470 };
void canny_edge(int ohm);
Mat hough_lines(Mat edge);
void hsv(int ohm);

int main(void) {
	int ohmCnt = 0, ohmSize = sizeof(ohm) / sizeof(int);
	int k;
	while (ohmCnt != ohmSize) {
		canny_edge(ohm[ohmCnt]);

		hsv(ohm[ohmCnt]);

		ohmCnt++;
		k = waitKey();
		if (k == 27) break;
	}
}
void canny_edge(int ohm) {
	string str = to_string(ohm) + "ohm-3.jpg";
	Mat src = imread(str, IMREAD_COLOR);
	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Mat src_gray;
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	Mat dst1, dst2, dst3;
	Canny(src_gray, dst1, 50, 100);
	Canny(src_gray, dst2, 50, 150);
	Canny(src_gray, dst3, 30, 200);

	Mat dst_line1, dst_line2, dst_line3;
	dst_line1 = hough_lines(dst1);
	dst_line2 = hough_lines(dst2);
	dst_line3 = hough_lines(dst3);
//	imshow("dst1", dst1);	imshow("dst2", dst2);	imshow("dst3", dst3);
	

	// 이미지 이어 붙이기 
	
//	resize(src, src, Size(2 * src.cols, 2 * src.rows));
//	resize(dst1, dst1, Size(2 * dst1.cols, 2 * dst1.rows));
//	resize(dst2, dst2, Size(2 * dst2.cols, 2 * dst2.rows));
//	resize(dst3, dst3, Size(2 * dst3.cols, 2 * dst3.rows));
	
	

	Mat dst;
	hconcat(dst1, dst2, dst);
	hconcat(dst, dst3, dst);
	cvtColor(dst, dst, COLOR_GRAY2BGR);

	Mat img;
	hconcat(src, dst, img);


	// 직선 검출 이미지 이어 붙이기


	Mat dst_line;
	cvtColor(src_gray, dst_line, COLOR_GRAY2BGR);
	hconcat(dst_line, dst_line1, dst_line);
	hconcat(dst_line, dst_line2, dst_line);
	hconcat(dst_line, dst_line3, dst_line);

	vconcat(img, dst_line, img);

	str = to_string(ohm) + "ohm";
	resize(img, img, Size(2 * img.cols, 2 * img.rows));
//	imshow(str, img);
	imshow("img", img);
}
Mat hough_lines(Mat edge) {

	vector<Vec2f> lines;
	HoughLines(edge, lines, 1, CV_PI / 180, 25);

	Mat dst;
	cvtColor(edge, dst, COLOR_GRAY2BGR);

//	cout << "size = " << lines.size() << endl;
	for (size_t i = 0; i < lines.size(); i++) {
		float r = lines[i][0], t = lines[i][1];
		double cos_t = cos(t), sin_t = sin(t);

		if (cos_t < 0.9) continue;

		double x0 = r * cos_t, y0 = r * sin_t;
		double alpha = 1000;

		Point pt1(cvRound(x0 + alpha * (-sin_t)), cvRound(y0 + alpha * cos_t));
		Point pt2(cvRound(x0 - alpha * (-sin_t)), cvRound(y0 - alpha * cos_t));
		line(dst, pt1, pt2, Scalar(0, 0, 255), 1, LINE_AA);
	//	cout << "cos_t = " << cos_t << ", sin_t = " << sin_t << endl;
	//	cout << "r = " << r << ", x0 = " << x0 << ", y0 = " << y0 << endl;
	//	cout << "pt1 = " << pt1 << ", pt2 = " << pt2 << endl << endl;
	//	imshow("dst", dst);		waitKey();
	}

	return dst;
}
void hsv(int ohm) {
	string str = to_string(ohm) + "ohm-3.jpg";
	Mat src = imread(str, IMREAD_COLOR);

	Mat src_hsv;

	cvtColor(src, src_hsv, COLOR_BGR2HSV);

	Mat mask, dst;

	Scalar lower = Scalar(10, 0, 0);
	Scalar upper = Scalar(22, 255, 255);

	inRange(src_hsv, lower, upper, mask);
	bitwise_and(src, src, dst, mask);

	resize(dst, dst, Size(3 * dst.cols, 3 * dst.rows));
	imshow("hsv", dst);
}
*/

// hsv 분석 트랙바
int lower_hue = 0, upper_hue = 179;
int lower_s = 80, upper_s = 255;
int lower_v = 0, upper_v = 255;
Mat src, src_hsv, mask, close, dst;
void on_hue_changed(int, void*);
int main(void)
{
	src = imread("kkk3.jpg", IMREAD_COLOR);
	if (src.empty()) { cerr << "Image load failed!" << endl; return -1; }
	resize(src, src, Size(2 * src.cols, 2 * src.rows));
	cvtColor(src, src_hsv, COLOR_BGR2HSV);

	imshow("src", src);
	namedWindow("mask");
	createTrackbar("Lower H", "mask", &lower_hue, 179, on_hue_changed);
	createTrackbar("Upper H", "mask", &upper_hue, 179, on_hue_changed);

	createTrackbar("Lower S", "mask", &lower_s, 255, on_hue_changed);
	createTrackbar("Upper S", "mask", &upper_s, 255, on_hue_changed);

	createTrackbar("Lower V", "mask", &lower_v, 255, on_hue_changed);
	createTrackbar("Upper V", "mask", &upper_v, 255, on_hue_changed);
	on_hue_changed(0, 0);
	waitKey(0);
	return 0;
}
void on_hue_changed(int, void*) //트랙바 이벤트 핸들러
{
	Scalar lowerb(lower_hue, lower_s, lower_v);
	Scalar upperb(upper_hue, upper_s, upper_v);
	inRange(src_hsv, lowerb, upperb, mask);
	imshow("mask", mask);

	morphologyEx(mask, close, MORPH_CLOSE, Mat(), Point(-1, -1), 5);
	imshow("close", close);

	dst = src.clone();
	for (int x = 0; x < dst.cols; x++) {
		for (int y = 0; y < dst.rows; y++) {
			Vec3b& p = dst.at<Vec3b>(y, x);
			if (close.at<uchar>(y, x) == 0) {
				p[0] = 156, p[1] = 146, p[2] = 54;
			}
		}
	}
	imshow("dst", dst);


	cout << "낮음 ㅣ " << lowerb << endl;
	cout << "높음 ㅣ " << upperb << endl << endl;
}
/*
int main(void) {
	vector<Mat> v;
	for (int i = 1; i <= 8; i++) {
		string str = "aohm-" + to_string(i) + ".jpg";
		Mat m = imread(str, IMREAD_COLOR);
		if (m.empty()) {
			cerr << "h" << endl << endl;
			return -1;
		}
		resize(m, m, Size(100, 40));
		v.push_back(m);
	}
	cout << ":" << endl;
	Mat up, down, dst;
	for (int i = 1; i <= 3; i++) {
		hconcat(v[i], v[i + 1], up);
		hconcat(v[i+4], v[i + 5], down);
	}
	imshow("d", down);
	imshow("u", up);
//	vconcat(up, down, dst);
//	imshow("dst", dst);
	waitKey();
}
*/

