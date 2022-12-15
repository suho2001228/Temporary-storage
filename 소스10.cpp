#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace cv::ml;
using namespace std;
int ohm[] = { 56,68,75,82,91,100,330,470 };
string color[] = { "brack","brown","red","orange","yellow","green","blue","purple","gray","white" };
int i_cnt = 0;
Mat getHist(const Mat& hist);
Mat calcHist(const Mat& img);
int main(void) {
	for (int icnt = 5; icnt <= 34; icnt++) {
	//	string str = to_string(ohm[i_cnt]) + "ohm-3.jpg";
		string str = "cohm-" + to_string(icnt) + ".jpg";
		Mat src = imread(str, IMREAD_COLOR);
		resize(src, src, Size(2 * src.cols, 2 * src.rows));
		if (src.empty()) {
			cerr << "에러" << endl;
			return -1;
		}

		imshow("src", src);
		Mat rect_hsv;
		cvtColor(src, rect_hsv, COLOR_BGR2HSV);

		imshow("rect_hsv", rect_hsv);

		Mat hist = calcHist(rect_hsv);
		Mat hist_img = getHist(hist);

		imshow("srcHist", hist_img);

		/*
		Mat rect_result(rect_hsv.rows, rect_hsv.cols, CV_8UC3, Scalar(0, 0, 0));
		vector<Mat> color;
		for (int color_cnt = 0; color_cnt < 11; color_cnt++) {
			color.push_back(rect_result.clone());
			//	imshow("color_test", color[color_cnt]);
		}
		color[0] = Scalar(255, 255, 255);
		*/

		// s > 45인 영역 찾기 color[10]
		Mat rect_s45(rect_hsv.rows, rect_hsv.cols, CV_8UC1, Scalar(0));
		//	Mat rect_result(rect_hsv.rows, rect_hsv.cols, CV_8UC1, Scalar(0));
		for (int x = 0; x < rect_hsv.cols; x++) {
			for (int y = 0; y < rect_hsv.rows; y++) {
				Vec3b& p = rect_hsv.at<Vec3b>(y, x);
				//	uchar& q = rect_result.at<uchar>(y, x);
				//	Vec3b& q = rect_result.at<Vec3b>(y, x);
				uchar r = 0, g = 0, b = 0;

				uchar& s45 = rect_s45.at<uchar>(y, x);
			//	int color_num;
				if (p[1] >= 80) {
					s45 = 255;
				}
			}
		}
		imshow("s45", rect_s45);
		Mat rect_s45_2(rect_s45.rows * 2, rect_s45.cols * 2, CV_8UC1, Scalar(0));
		for (int x = 0; x < rect_s45.cols; x++) {
			for (int y = 0; y < rect_s45.rows; y++) {
				if (rect_s45.at<uchar>(y, x) == 255) {
					rect_s45_2.at<uchar>(y + rect_s45.rows / 2, x + rect_s45.cols / 2) = 255;
				}
			}
		}
		morphologyEx(rect_s45_2, rect_s45_2, MORPH_CLOSE, Mat(), Point(-1, -1), 6);
		imshow("s45_2", rect_s45_2);

		Mat rect_s45_3 = rect_s45_2(Rect(rect_s45.cols / 2, rect_s45.rows / 2, rect_s45.cols, rect_s45.rows));
		imshow("s45_3", rect_s45_3); // 이 영역안에서 hsv 분석함.

		vector<Mat> color;
		vector<Mat> aaa;
		for (int i = 0; i < 10; i++) {
			color.push_back(Mat(rect_s45_3.rows, rect_s45_3.cols, CV_8UC3, Scalar(156, 146, 54)));
			aaa.push_back(Mat(rect_s45_3.rows, rect_s45_3.cols, CV_8UC1, Scalar(0)));
		}
		for (int x = 0; x < rect_s45_3.cols; x++) {
			for (int y = 0; y < rect_s45_3.rows; y++) {
				if (rect_s45_3.at<uchar>(y, x) == 255) {
					Vec3b& p = rect_hsv.at<Vec3b>(y, x);
					uchar r = 0, g = 0, b = 0;


					// 갈색 1
					if ((p[0] <= 5) && (p[1] >= 80 && p[1] <= 140) && (p[2] >= 60 && p[2] <= 100)) {
						r = 103, g = 0, b = 0;
						int color_num = 1;
						Vec3b& q = color[color_num].at<Vec3b>(y, x);
						q[0] = b, q[1] = g, q[2] = r;
						aaa[color_num].at<uchar>(y, x) = 255;
					}

					// 빨강 2
					if ((p[0] <= 5 || p[0] >= 175) && (p[1] >= 140) && (p[2] >= 90)) {
						r = 255, g = 0, b = 0;
						int color_num = 2;
						Vec3b& q = color[color_num].at<Vec3b>(y, x);
						q[0] = b, q[1] = g, q[2] = r;
						aaa[color_num].at<uchar>(y, x) = 255;
					}

					// 주황 3
					if ((p[0] >= 8 && p[0] <= 10) && (p[1] >= 157) && (p[2] >= 100)) {
						r = 255, g = 94, b = 0;
						int color_num = 3;
						Vec3b& q = color[color_num].at<Vec3b>(y, x);
						q[0] = b, q[1] = g, q[2] = r;
						aaa[color_num].at<uchar>(y, x) = 255;
					}

					// 노랑 4
					if ((p[0] >= 16 && p[0] <= 24) && (p[1] >= 130)) {
						r = 255, g = 228, b = 0;
						int color_num = 4;
						Vec3b& q = color[color_num].at<Vec3b>(y, x);
						q[0] = b, q[1] = g, q[2] = r;
						aaa[color_num].at<uchar>(y, x) = 255;
					}

					// 초록 5
					if ((p[0] >= 25 && p[0] <= 80)) {
						r = 29, g = 219, b = 22;
						int color_num = 5;
						Vec3b& q = color[color_num].at<Vec3b>(y, x);
						q[0] = b, q[1] = g, q[2] = r;
						aaa[color_num].at<uchar>(y, x) = 255;
					}
					// 검정 0
					else if ((p[2] <= 60)) {
							r = 0, g = 0, b = 0;
							int color_num = 0;
							Vec3b& q = color[color_num].at<Vec3b>(y, x);
							q[0] = b, q[1] = g, q[2] = r;
							aaa[color_num].at<uchar>(y, x) = 255;
					}

					// 파랑 6
					if ((p[0] >= 100 && p[0] <= 140) && (p[1] <= 120) && (p[2] <= 80)) {
						r = 0, g = 84, b = 255;
						int color_num = 6;
						Vec3b& q = color[color_num].at<Vec3b>(y, x);
						q[0] = b, q[1] = g, q[2] = r;
						aaa[color_num].at<uchar>(y, x) = 255;
					}

					// 보라 7
					if ((p[0] <= 8 || p[0] >= 170)&& (p[1] >= 60 && p[1] <= 130) && (p[2] <= 115)) {
						r = 95, g = 0, b = 255;
						int color_num = 7;
						Vec3b& q = color[color_num].at<Vec3b>(y, x);
						q[0] = b, q[1] = g, q[2] = r;
						aaa[color_num].at<uchar>(y, x) = 255;
					}

					// 회색 8
					if ((p[0] <= 20) && (p[1] >= 21 && p[1] <= 96) && (p[2] >= 71 && p[2] <= 95)) {
						r = 140, g = 140, b = 140;
						int color_num = 8;
						Vec3b& q = color[color_num].at<Vec3b>(y, x);
						q[0] = b, q[1] = g, q[2] = r;
						aaa[color_num].at<uchar>(y, x) = 255;
					}

					// 흰색 9
					if ((p[1] >= 35 && p[1] <= 80) && (p[2] >= 130 && p[2] <= 180)) {
						r = 246, g = 246, b = 246;
						int color_num = 9;
						Vec3b& q = color[color_num].at<Vec3b>(y, x);
						q[0] = b, q[1] = g, q[2] = r;
						aaa[color_num].at<uchar>(y, x) = 255;
					}
				}
			}
		}

		 
		Mat labels, stats, centroids;
		vector<int> i_size, i_color;
		vector<double> i_centroid;
		for (int i = 0; i < 10; i++) {
			int cnt = connectedComponentsWithStats(aaa[i], labels, stats, centroids);
			for (int j = 1; j < cnt; j++) {
				int* p = stats.ptr<int>(j);
				if (p[4] < 20) continue;
				i_size.push_back(stats.at<int>(j, 4));
				i_centroid.push_back(centroids.at<double>(j, 0));
				i_color.push_back(i);
			}
		}

		int temp_size, temp_color;
		double temp_centroid;
	//	cout << "! : " <<i_size.size() << endl;
		for (int i = 0; i < i_size.size() - 1; i++) {
			for (int j = 0; j < i_size.size() - 1 - i; j++) {
				if (i_size[j] < i_size[j + 1]) {
					temp_size = i_size[j];
					i_size[j] = i_size[j + 1];
					i_size[j + 1] = temp_size;

					temp_color = i_color[j];
					i_color[j] = i_color[j + 1];
					i_color[j + 1] = temp_color;

					temp_centroid = i_centroid[j];
					i_centroid[j] = i_centroid[j + 1];
					i_centroid[j + 1] = temp_centroid;
				}
			}
		}

		i_size.resize(3), i_color.resize(3), i_centroid.resize(3);

		for (int i = 0; i < i_size.size() - 1; i++) {
			for (int j = 0; j < i_size.size() - 1 - i; j++) {
				if (i_centroid[j] < i_centroid[j + 1]) {
					temp_size = i_size[j];
					i_size[j] = i_size[j + 1];
					i_size[j + 1] = temp_size;

					temp_color = i_color[j];
					i_color[j] = i_color[j + 1];
					i_color[j + 1] = temp_color;

					temp_centroid = i_centroid[j];
					i_centroid[j] = i_centroid[j + 1];
					i_centroid[j + 1] = temp_centroid;
				}
			}
		}



		for (int i = 0; i < 3; i++) {
			cout << "color : " << i_color[i] << ", size : " << i_size[i] << ", centroid : " << i_centroid[i] << endl;
		}

		if (i_centroid[2] < rect_s45.cols - i_centroid[0]) {
			cout << "-> ohm = " << (i_color[2] * 10 + i_color[1]) * pow(10, i_color[0]) << endl;
		}
		else {
			cout << "<- ohm = " << (i_color[0] * 10 + i_color[1]) * pow(10, i_color[2]) << endl;
		}

		cout << endl;


		
		Mat color_up, color_down;
		hconcat(color[0], color[1], color_up);
		for (int i = 2; i < 5; i++) {
			hconcat(color_up, color[i], color_up);
		}
		hconcat(color[5], color[6], color_down);
		for (int i = 7; i < 10; i++) {
			hconcat(color_down, color[i], color_down);
		}
		Mat color_result;
		//	imshow("color_up", color_up);
		//	imshow("color_down", color_down);
		vconcat(color_up, color_down, color_result);
		//	imshow("rect_result", rect_result);

		for (int i = 1; i < 5; i++) {
			line(color_result, Point(i * color_result.cols / 5, 0), Point(i * color_result.cols / 5, color_result.rows - 1), Scalar(255, 255, 255));
		}
		line(color_result, Point(0, color_result.rows / 2), Point(color_result.cols - 1, color_result.rows / 2), Scalar(255, 255, 255));

		imshow("color_result", color_result);

		
		int k = waitKey();
		if (k == 27) break;
		else if (k == 'a') icnt -= 2;
	}
}
Mat calcHist(const Mat& img) {
	vector<Mat> hsv;
	split(img, hsv);

	Mat hist;
	int channels[] = { 0 };
	int dims = 1;
	const int histSize[] = { 180 };
	float level[] = { 0,180 };
	const float* ranges[] = { level };

	calcHist(&hsv[0], 1, channels, noArray(), hist, dims, histSize, ranges);

	return hist;
}
Mat getHist(const Mat& hist) {
	CV_Assert(hist.type() == CV_32FC1);
	CV_Assert(hist.size() == Size(1, 180));

	double histMax;
	minMaxLoc(hist, 0, &histMax);

	Mat imgHist(1000, 180, CV_8UC1, Scalar(255));
	for (int i = 0; i < 180; i++) {
		line(imgHist, Point(i, 1000),
			Point(i, 1000 - cvRound(hist.at<float>(i, 0) * 1000 / histMax)), Scalar(0));
	}

	return imgHist;
}