#ifndef _DIGIT_CUT_
#define _DIGIT_CUT_
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
namespace calculator {
	class Cut {
	public:
		static Mat y_cut(Mat m);
		static Mat x_cut(Mat m);
		static Mat c_cut(Mat m, int* p, double* c);
	};
}
#endif