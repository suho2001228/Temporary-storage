#ifndef _DIGIT_
#define _DIGIT_
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
namespace calculator {
	class Digit {
	public:
		static bool minus_(int* p);
		static bool one(int* p);
		static bool divide_(Mat m);
		static bool times_(Mat m, int* p, double* centroid);
		static bool plus_(Mat m, int* p, double* centroid);
		static bool po(int* p);
		static bool pi(Mat x, int x_avg, Mat y, int y_avg);
		static bool seven_route(Mat ym, int y_avg, int y9_y0);
		static bool route(Mat c);
		static bool par_left(Mat x, int x_avg, int l_right);
		static bool par_right(Mat x, int x_avg, int l_left);
		static bool three_five(Mat y, int y_avg);
		static bool two(Mat y, int y_avg);
		static bool zero(Mat c, int c_avg, Mat y, int y_avg, Mat x, int x_avg);
		static bool four(Mat x, int x_avg, Mat y, int y_avg);
		static bool nine(Mat x, int x_avg);
		static bool six(Mat x, int x_avg);
		static bool e_(Mat c);
	};
}
#endif
