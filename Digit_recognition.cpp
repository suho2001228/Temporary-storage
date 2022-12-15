#include "Digit_recognition.h"
namespace calculator {
	bool Digit::minus_(int* p) {
		if (p[2] / p[3] >= 4 && p[3] <= 30) {
			cout << "p2 : " << p[2] << "p3 : " << p[3] << endl;
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::one(int* p) {
		if (p[3] / p[2] >= 3) {
			cout << "p2 : " << p[2] << "p3 : " << p[3] << endl;
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::divide_(Mat m) {
		Mat cm = m;
		if (cm.at<int>(0, 1) > 4 * cm.at<int>(0, 0) && cm.at<int>(1, 0) > 4 * cm.at<int>(1, 1)) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::times_(Mat m, int* p, double* centroid) {
		Mat img = m;
		int x = 0;
		for (int j = centroid[1] - 0.1 * p[3]; j < centroid[1] + 0.1 * p[3]; j++) {
			for (int i = p[0]; i < p[0] + 0.1 * p[2]; i++) {
				if (img.at<uchar>(j, i) > 0) {
					x++;
					break;
				}
			}
			for (int i = p[0] + 0.9 * p[2]; i < p[0] + p[2]; i++) {
				if (img.at<uchar>(j, i) > 0) {
					x++;
					break;
				}
			}
		}
		for (int i = centroid[0] - 0.1 * p[2]; i < centroid[0] + 0.1 * p[2]; i++) {
			for (int j = p[1]; j < p[1] + 0.1 * p[3]; j++) {
				if (img.at<uchar>(j, i) > 0) {
					x++;
					break;
				}
			}
			for (int j = p[1] + 0.9 * p[3]; j < p[1] + p[3]; j++) {
				if (img.at<uchar>(j, i) > 0) {
					x++;
					break;
				}
			}
		}
		if (x == 0) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::plus_(Mat m, int* p, double* centroid) {
		Mat img = m;
		int plus = 0;
		for (int j = p[1]; j < p[1] + 0.2 * p[3]; j++) {
			for (int i = p[0]; i < p[0] + 0.2 * p[2]; i++) {
				if (img.at<uchar>(j, i) > 0) {
					plus++;
					break;
				}
			}
			for (int i = p[0] + 0.8 * p[2]; i < p[0] + p[2]; i++) {
				if (img.at<uchar>(j, i) > 0) {
					plus++;
					break;
				}
			}
		}
		for (int j = p[1] + 0.8 * p[3]; j < p[1] + p[3]; j++) {
			for (int i = p[0]; i < p[0] + 0.2 * p[2]; i++) {
				if (img.at<uchar>(j, i) > 0) {
					plus++;
					break;
				}
			}
			for (int i = p[0] + 0.8 * p[2]; i < p[0] + p[2]; i++) {
				if (img.at<uchar>(j, i) > 0) {
					plus++;
					break;
				}
			}
		}
		if (plus == 0) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::po(int* p) {
		if (p[2] < 30 && p[3] < 30) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::pi(Mat x, int x_avg, Mat y, int y_avg) {
		Mat xm = x, ym = y;
		if (ym.at<int>(0, 0) > 3 * y_avg && (ym.at<int>(8, 0) > y_avg || ym.at<int>(9, 0) > y_avg) &&
			(xm.at<int>(0, 1) > x_avg || xm.at<int>(0, 2) > x_avg || xm.at<int>(0, 3) > x_avg) &&
			(xm.at<int>(0, 5) > x_avg || xm.at<int>(0, 6) > x_avg)) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::seven_route(Mat y, int y_avg, int y9_y0) {
		Mat ym = y;
		if (ym.at<int>(9, 0) < y_avg && y9_y0 > 1.5) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::route(Mat c) {
		Mat cm = c;
		if (cm.at<int>(1, 1) == 0) {
			return true;
		}
		else {
			return false;
		}
	}

	bool Digit::par_left(Mat x, int x_avg, int l_right) {
		Mat xm = x;
		if (xm.at<int>(0, 0) > 2. * x_avg && l_right == 0) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::par_right(Mat x, int x_avg, int l_left) {
		Mat xm = x;
		if (xm.at<int>(0, 9) > 2. * x_avg && l_left == 0) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::three_five(Mat y, int y_avg) {
		Mat ym = y;
		if (ym.at<int>(3, 0) > y_avg || ym.at<int>(4, 0) > y_avg || ym.at<int>(5, 0) > y_avg || ym.at<int>(6, 0) > y_avg) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::two(Mat y, int y_avg) {
		Mat ym = y;
		if ((ym.at<int>(7, 0) + ym.at<int>(8, 0) + ym.at<int>(9, 0)) > 4 * y_avg) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::zero(Mat c, int cm_avg, Mat y, int y_avg, Mat x, int x_avg) {
		Mat cm = c, ym = y, xm = x;
		if (abs(cm.at<int>(0, 0) - cm_avg) < 0.4 * cm_avg && abs(cm.at<int>(0, 1) - cm_avg) < 0.4 * cm_avg &&
			abs(cm.at<int>(1, 0) - cm_avg) < 0.4 * cm_avg && abs(cm.at<int>(1, 1) - cm_avg) < 0.4 * cm_avg &&
			ym.at<int>(3, 0) < y_avg && ym.at<int>(4, 0) < y_avg && ym.at<int>(5, 0) < y_avg &&
			ym.at<int>(6, 0) < y_avg && ym.at<int>(7, 0) < y_avg &&
			xm.at<int>(0, 0) > x_avg && xm.at<int>(0, 9) > x_avg) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::four(Mat x, int x_avg, Mat y, int y_avg) {
		Mat xm = x, ym = y;
		if (xm.at<int>(0, 3) + xm.at<int>(0, 4) + xm.at<int>(0, 5) + xm.at<int>(0, 6) > 5 * x_avg ||
			ym.at<int>(3, 0) + ym.at<int>(4, 0) + ym.at<int>(5, 0) + ym.at<int>(6, 0) > 5 * x_avg) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::nine(Mat x, int x_avg) {
		Mat xm = x;
		if ((xm.at<int>(0, 9) + xm.at<int>(0, 8) > 2.5 * x_avg || xm.at<int>(0, 8) + xm.at<int>(0, 7) > 2.5 * x_avg)) {
			return true;
		}
		else {
			return false;
		}
	}

	bool Digit::six(Mat x, int x_avg) {
		Mat xm = x;
		if ((xm.at<int>(0, 0) + xm.at<int>(0, 1) + xm.at<int>(0, 2) > 2.8 * x_avg)) {
			return true;
		}
		else {
			return false;
		}
	}
	bool Digit::e_(Mat c) {
		Mat cm = c;
		if (cm.at<int>(0, 1) > cm.at<int>(1, 1)) {
			return true;
		}
		else {
			return false;
		}
	}
}
