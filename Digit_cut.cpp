#include"Digit_cut.h"
namespace calculator {
	Mat Cut::y_cut(Mat m) { // 0~9 : 각 번호의 합, 10 : 전체합
		Mat img = m;
		Mat ym = Mat::zeros(11, 1, CV_32SC1);
		int y_sum = 0;
		for (int y_num = 0; y_num < 10; y_num++) {
			for (int i = 0; i < img.cols; i++) {
				for (int j = 0.1 * y_num * img.rows; j < 0.1 * (y_num + 1) * img.rows; j++) {
					if (img.at<uchar>(j, i) > 0) {
						ym.at<int>(y_num, 0)++;
					}
				}
			}
			//			cout << y_num << "번째 : " << ym.at<int>(y_num, 0) << endl;
			y_sum += ym.at<int>(y_num, 0);
		}
		ym.at<int>(10, 0) = y_sum;
		return ym;
	}
	Mat Cut::x_cut(Mat m) {
		Mat img = m;
		Mat xm = Mat::zeros(1, 11, CV_32SC1);
		int x_sum = 0;
		for (int x_num = 0; x_num < 10; x_num++) {
			for (int j = 0; j < img.rows; j++) {
				for (int i = 0.1 * x_num * img.cols; i < 0.1 * (x_num + 1) * img.cols; i++) {
					if (img.at<uchar>(j, i) > 0) {
						xm.at<int>(0, x_num)++;
					}
				}
			}
			//			cout << x_num << "번째 : " << xm.at<int>(0, x_num) << endl;
			x_sum += xm.at<int>(0, x_num);
		}
		xm.at<int>(0, 10) = x_sum;
		return xm;
	}
	Mat Cut::c_cut(Mat m, int* p, double* centroid) {
		Mat img = m;
		Mat cm = Mat::zeros(2, 2, CV_32SC1);
		for (int i = p[0]; i < centroid[0]; i++) {
			for (int j = p[1]; j < centroid[1]; j++) {
				if (img.at<uchar>(j, i) > 0) {
					cm.at<int>(0, 0)++;
				}
			}
			for (int j = centroid[1]; j < p[1] + p[3]; j++) {
				if (img.at<uchar>(j, i) > 0) {
					cm.at<int>(1, 0)++;
				}
			}
		}
		for (int i = centroid[0]; i < p[0] + p[2]; i++) {
			for (int j = p[1]; j < centroid[1]; j++) {
				if (img.at<uchar>(j, i) > 0) {
					cm.at<int>(0, 1)++;
				}
			}
			for (int j = centroid[1]; j < p[1] + p[3]; j++) {
				if (img.at<uchar>(j, i) > 0) {
					cm.at<int>(1, 1)++;
				}
			}
		}
		return cm;
	}
}