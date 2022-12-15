#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
using namespace std;
using namespace cv;
Mat ohm_hsv(Mat);
int ohm_hsv_color(Mat, Mat);

const float CONFIDENCE_THRESHOLD = 0.5;
const float NMS_THRESHOLD = 0.5;
const int NUM_CLASSES = 1;
// colors for bounding boxes
const cv::Scalar colors[] = {
{0, 255, 255}
};
const auto NUM_COLORS = sizeof(colors) / sizeof(colors[0]);
int main()
{
	std::vector<std::string> class_names = { "ohm" };
	auto net = cv::dnn::readNetFromDarknet("yolov4-ohm.cfg", "yolov4-ohm_final.weights");
		//net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
		//net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
		net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
	auto output_names = net.getUnconnectedOutLayersNames();

	int iohm_cnt = 1;

	for (int i_cnt = 3; i_cnt <= 8; i_cnt++) { // 분석할 사진 수만큼 반복
		
		string str = "aaa";	str += to_string(i_cnt) + ".jpg"; // 파일명 
	//	string str = "qqqq.jpg";
		clock_t start, end;
		double duration;
		start = clock();
		cv::Mat frame = imread(str);
		cv::resize(frame, frame, Size(320, 320)); // 320x320으로 크기 조정
		cv::Mat blob;
		std::vector<cv::Mat> detections;
		if (frame.empty())
		{
			cerr << "frame empty" << endl; return -1;
		}
		cv::dnn::blobFromImage(frame, blob, 1 / 255.f, cv::Size(320, 320), cv::Scalar(),
			true, false, CV_32F);
		net.setInput(blob);
		net.forward(detections, output_names);
		std::vector<int> indices[NUM_CLASSES];
		std::vector<cv::Rect> boxes[NUM_CLASSES];
		std::vector<float> scores[NUM_CLASSES];
		end = clock();
		duration = (double)(end - start) / CLOCKS_PER_SEC;

		cout << "yolov4 : " << duration << endl; // yolov4가 객체를 찾는 시간을 측정함
		for (auto& output : detections)
		{
			const auto num_boxes = output.rows;
			for (int i = 0; i < num_boxes; i++)
			{
				auto x = output.at<float>(i, 0) * frame.cols;
				auto y = output.at<float>(i, 1) * frame.rows;
				auto width = output.at<float>(i, 2) * frame.cols;
				auto height = output.at<float>(i, 3) * frame.rows;
				cv::Rect rect(x - width / 2, y - height / 2, width, height);
				for (int c = 0; c < NUM_CLASSES; c++)
				{
					auto confidence = *output.ptr<float>(i, 5 + c);
					if (confidence >= CONFIDENCE_THRESHOLD)
					{
						boxes[c].push_back(rect);
						scores[c].push_back(confidence);
					}
				}
			}
		}
		for (int c = 0; c < NUM_CLASSES; c++)
			cv::dnn::NMSBoxes(boxes[c], scores[c], 0.0, NMS_THRESHOLD,
				indices[c]);

		vector<Point> vec_rect;


		for (int c = 0; c < NUM_CLASSES; c++) // 클래스가 1개임으로 for문은 1번 반복
		{
			
			for (int i = 0; i < indices[c].size(); ++i) // 찾은 객체(저항) 수 만큼 반복
			{
				start = clock();
				const auto color = colors[0];
				auto idx = indices[c][i];
				const auto& rect = boxes[c][idx];

				// 객체 영역 좌표값 
			//	cout << "객체" << i_cnt << "번째 좌표 : " << cv::Point(rect.x, rect.y) << ", " << cv::Point(rect.x + rect.width, rect.y + rect.height) << endl;
				cv::Mat ohm;
				frame(Rect(rect.x, rect.y, rect.width, rect.height)).copyTo(ohm); // 객체 영역 깊은 복사
				string ohm_cnt = "ohm-" + to_string(i);
//				cv::resize(ohm, ohm, Size(3 * ohm.cols, 3 * ohm.rows));
//				cv::imshow(ohm_cnt, ohm);

			//	string write_str = "cohm-" + to_string(iohm_cnt + 4) + ".jpg";
			//	cv::imwrite(write_str, ohm);
				iohm_cnt++;
			//	hsv_ohm(rect_hsv);

				// 저항 분석 시작
				resize(ohm, ohm, Size(2 * ohm.cols, 2 * ohm.rows)); // 저항의 크기가 작으므로 크기를 4배 키움
				Mat rect_hsv;
				cvtColor(ohm, rect_hsv, COLOR_BGR2HSV); // RGB 저항 이미지를 HSV 이미지로 변경
			//	imshow("rect_hsv", rect_hsv);

				Mat rect_s45_3 = ohm_hsv(rect_hsv); // HSV 영역에서 저항의 영역을 이진화 하여 저항 외 영역을 제거함.


				int ohm_result = ohm_hsv_color(rect_s45_3, rect_hsv); // HSV 영역에서 10가지의 띠 색(현재 6가지 밖에 분류하지 못함.)을 분석하여 저항의 크기를 계산함.
			// 저항 분석  끝

				// 객체 영역을 바운딩 박스로 표시하고 저항의 크기를 출력함.
				cv::rectangle(frame, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 3);
				std::string label_str = cv::format("%d", ohm_result);
				int baseline;
				auto label_bg_sz = cv::getTextSize(label_str, cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
				cv::rectangle(frame, cv::Point(rect.x, rect.y - label_bg_sz.height - baseline - 10), cv::Point(rect.x + label_bg_sz.width, rect.y), color, cv::FILLED);
				cv::putText(frame, label_str, cv::Point(rect.x, rect.y - baseline - 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(0, 0, 0));
				
				end = clock();
				duration = (double)(end - start) / CLOCKS_PER_SEC;
				cout << "저항 띠 분석 시간: " << duration << endl; // 객체(저항) 하나 분석 시간 측정
			}
		}
		cv::namedWindow("output");
		cv::imshow("output", frame);
		cv::waitKey(0);
	}
	return 0;
}
Mat ohm_hsv(Mat rect_hsv) {
	Mat rect_s45(rect_hsv.rows, rect_hsv.cols, CV_8UC1, Scalar(0)); // 객체 영역과 같은 크기의 그레이 이미지를 생성하고 검정색으로 초기화.
	for (int x = 0; x < rect_hsv.cols; x++) { // 영상의 모든 좌표에서
		for (int y = 0; y < rect_hsv.rows; y++) {
			Vec3b& p = rect_hsv.at<Vec3b>(y, x); // HSV값을 확인하고
			uchar r = 0, g = 0, b = 0;

			uchar& s45 = rect_s45.at<uchar>(y, x);
			//	int color_num;
			if (p[1] >= 80) { // S > 80 이상인 부분 (S < 80인 영역은 배경이 대부분 포함됨.)을 흰색으로 변경 / 이진화 하는 과정
				s45 = 255;
			}
		}
	}
//		imshow("s45", rect_s45);

	// 하지만 이 과정으로 인해 파란색, 회색, 흰색과 같은 띠 또한 제거됨. 이를 해결하기 위해 닫기 연산을 진행함.
	// 원본 이미지로 닫기 연산을 진행하면 끝에 부분에서 영향을 받기 때문에 원본 이미지의 4배 크기에 이미지를 생성하고 
	// 원본이미지를 가운데에 넣어 끝 부분에도 닫기 연산이 잘 되도록 해줌.
	Mat rect_s45_2(rect_s45.rows * 2, rect_s45.cols * 2, CV_8UC1, Scalar(0)); 
	for (int x = 0; x < rect_s45.cols; x++) {
		for (int y = 0; y < rect_s45.rows; y++) {
			if (rect_s45.at<uchar>(y, x) == 255) {
				rect_s45_2.at<uchar>(y + rect_s45.rows / 2, x + rect_s45.cols / 2) = 255;
			}
		}
	}
	morphologyEx(rect_s45_2, rect_s45_2, MORPH_CLOSE, Mat(), Point(-1, -1), 6); // 닫기 연산 진행
	//		imshow("s45_2", rect_s45_2);

	// 4배 키운 이지미에서 원본 영역을 다시 추출함.
	Mat rect_s45_3 = rect_s45_2(Rect(rect_s45.cols / 2, rect_s45.rows / 2, rect_s45.cols, rect_s45.rows));
	//		imshow("s45_3", rect_s45_3); // 이 영역안에서 hsv 분석함.

	return rect_s45_3;
}
int ohm_hsv_color(Mat rect_s45_3, Mat rect_hsv) {
	vector<Mat> aaa; // 색 영역을 저장할 벡터
	for (int i = 0; i < 10; i++) {
		aaa.push_back(Mat(rect_s45_3.rows, rect_s45_3.cols, CV_8UC1, Scalar(0))); // 10개의 벡터를 생성하고 0으로 초기화 함.
	}
	for (int x = 0; x < rect_s45_3.cols; x++) {
		for (int y = 0; y < rect_s45_3.rows; y++) {
			if (rect_s45_3.at<uchar>(y, x) == 255) { // 이진 영상의 좌표가 255인 경우 (저항 영역인 경우)
				Vec3b& p = rect_hsv.at<Vec3b>(y, x);

				// 각 영역의 색을 찾는 과정. HSV값을 분석함.
				// 갈색 1
				if ((p[0] <= 5) && (p[1] >= 80 && p[1] <= 140) && (p[2] >= 60 && p[2] <= 100)) {
					int color_num = 1;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// 빨강 2
				if ((p[0] <= 5 || p[0] >= 175) && (p[1] >= 140) && (p[2] >= 90)) {
					int color_num = 2;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// 주황 3
				if ((p[0] >= 8 && p[0] <= 10) && (p[1] >= 157) && (p[2] >= 100)) {
					int color_num = 3;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// 노랑 4
				if ((p[0] >= 16 && p[0] <= 24) && (p[1] >= 130)) {
					int color_num = 4;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// 초록 5
				if ((p[0] >= 25 && p[0] <= 80)) {
					int color_num = 5;
					aaa[color_num].at<uchar>(y, x) = 255;
				}
				// 검정 0
				else if ((p[2] <= 60)) {
					int color_num = 0;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// 파랑 6
				if ((p[0] >= 100 && p[0] <= 140) && (p[1] <= 120) && (p[2] <= 80)) {
					int color_num = 6;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// 보라 7
				if ((p[0] <= 8 || p[0] >= 170) && (p[1] >= 60 && p[1] <= 130) && (p[2] <= 115)) {
					int color_num = 7;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// 회색 8
				if ((p[0] <= 20) && (p[1] >= 21 && p[1] <= 96) && (p[2] >= 71 && p[2] <= 95)) {
					int color_num = 8;
				//	aaa[color_num].at<uchar>(y, x) = 255;
				}

				// 흰색 9
				if ((p[1] >= 35 && p[1] <= 80) && (p[2] >= 130 && p[2] <= 180)) {
					int color_num = 9;
					aaa[color_num].at<uchar>(y, x) = 255;
				}
			}
		}
	}

	// 이후 각 영역에서 검출된 색 영영에서 레이블링을 진행하여 얼마나 검출되었는지 확인함.
	Mat labels, stats, centroids;
	vector<int> i_size, i_color; // 레이블링된 영역의 크기, 색깔
	vector<double> i_centroid; // 무게중심을 저장함.
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

	// 버블 정렬을 이용하여 크키가 큰 순으로 크기, 색깔, 무게중심을 정렬함.
	// 
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

	// 크기 순으로 정렬 후 가장 큰 3개의 원소만 저장
	i_size.resize(3), i_color.resize(3), i_centroid.resize(3);

	// 이후 다시 버블정렬을 통해 무게중심의 좌표순으로 다시 정렬함.
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
	int ohm_result;

	/*
	for (int i = 0; i < 3; i++) {
		cout << "color : " << i_color[i] << ", size : " << i_size[i] << ", centroid : " << i_centroid[i] << endl;
	}
	*/
	
	// 무게중심을 통해 저항의 방향을 결정하고 저항의 크기를 계산함.
	if (i_centroid[2] < rect_s45_3.cols - i_centroid[0]) {
		ohm_result = (i_color[2] * 10 + i_color[1]) * pow(10, i_color[0]);
	}
	else {
		ohm_result = (i_color[0] * 10 + i_color[1]) * pow(10, i_color[2]);
	}
//	cout << "ohm 크기 : " << ohm_result << endl;
	return ohm_result;
}