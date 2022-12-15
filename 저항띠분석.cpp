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

	for (int i_cnt = 3; i_cnt <= 8; i_cnt++) { // �м��� ���� ����ŭ �ݺ�
		
		string str = "aaa";	str += to_string(i_cnt) + ".jpg"; // ���ϸ� 
	//	string str = "qqqq.jpg";
		clock_t start, end;
		double duration;
		start = clock();
		cv::Mat frame = imread(str);
		cv::resize(frame, frame, Size(320, 320)); // 320x320���� ũ�� ����
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

		cout << "yolov4 : " << duration << endl; // yolov4�� ��ü�� ã�� �ð��� ������
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


		for (int c = 0; c < NUM_CLASSES; c++) // Ŭ������ 1�������� for���� 1�� �ݺ�
		{
			
			for (int i = 0; i < indices[c].size(); ++i) // ã�� ��ü(����) �� ��ŭ �ݺ�
			{
				start = clock();
				const auto color = colors[0];
				auto idx = indices[c][i];
				const auto& rect = boxes[c][idx];

				// ��ü ���� ��ǥ�� 
			//	cout << "��ü" << i_cnt << "��° ��ǥ : " << cv::Point(rect.x, rect.y) << ", " << cv::Point(rect.x + rect.width, rect.y + rect.height) << endl;
				cv::Mat ohm;
				frame(Rect(rect.x, rect.y, rect.width, rect.height)).copyTo(ohm); // ��ü ���� ���� ����
				string ohm_cnt = "ohm-" + to_string(i);
//				cv::resize(ohm, ohm, Size(3 * ohm.cols, 3 * ohm.rows));
//				cv::imshow(ohm_cnt, ohm);

			//	string write_str = "cohm-" + to_string(iohm_cnt + 4) + ".jpg";
			//	cv::imwrite(write_str, ohm);
				iohm_cnt++;
			//	hsv_ohm(rect_hsv);

				// ���� �м� ����
				resize(ohm, ohm, Size(2 * ohm.cols, 2 * ohm.rows)); // ������ ũ�Ⱑ �����Ƿ� ũ�⸦ 4�� Ű��
				Mat rect_hsv;
				cvtColor(ohm, rect_hsv, COLOR_BGR2HSV); // RGB ���� �̹����� HSV �̹����� ����
			//	imshow("rect_hsv", rect_hsv);

				Mat rect_s45_3 = ohm_hsv(rect_hsv); // HSV �������� ������ ������ ����ȭ �Ͽ� ���� �� ������ ������.


				int ohm_result = ohm_hsv_color(rect_s45_3, rect_hsv); // HSV �������� 10������ �� ��(���� 6���� �ۿ� �з����� ����.)�� �м��Ͽ� ������ ũ�⸦ �����.
			// ���� �м�  ��

				// ��ü ������ �ٿ�� �ڽ��� ǥ���ϰ� ������ ũ�⸦ �����.
				cv::rectangle(frame, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 3);
				std::string label_str = cv::format("%d", ohm_result);
				int baseline;
				auto label_bg_sz = cv::getTextSize(label_str, cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
				cv::rectangle(frame, cv::Point(rect.x, rect.y - label_bg_sz.height - baseline - 10), cv::Point(rect.x + label_bg_sz.width, rect.y), color, cv::FILLED);
				cv::putText(frame, label_str, cv::Point(rect.x, rect.y - baseline - 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(0, 0, 0));
				
				end = clock();
				duration = (double)(end - start) / CLOCKS_PER_SEC;
				cout << "���� �� �м� �ð�: " << duration << endl; // ��ü(����) �ϳ� �м� �ð� ����
			}
		}
		cv::namedWindow("output");
		cv::imshow("output", frame);
		cv::waitKey(0);
	}
	return 0;
}
Mat ohm_hsv(Mat rect_hsv) {
	Mat rect_s45(rect_hsv.rows, rect_hsv.cols, CV_8UC1, Scalar(0)); // ��ü ������ ���� ũ���� �׷��� �̹����� �����ϰ� ���������� �ʱ�ȭ.
	for (int x = 0; x < rect_hsv.cols; x++) { // ������ ��� ��ǥ����
		for (int y = 0; y < rect_hsv.rows; y++) {
			Vec3b& p = rect_hsv.at<Vec3b>(y, x); // HSV���� Ȯ���ϰ�
			uchar r = 0, g = 0, b = 0;

			uchar& s45 = rect_s45.at<uchar>(y, x);
			//	int color_num;
			if (p[1] >= 80) { // S > 80 �̻��� �κ� (S < 80�� ������ ����� ��κ� ���Ե�.)�� ������� ���� / ����ȭ �ϴ� ����
				s45 = 255;
			}
		}
	}
//		imshow("s45", rect_s45);

	// ������ �� �������� ���� �Ķ���, ȸ��, ����� ���� �� ���� ���ŵ�. �̸� �ذ��ϱ� ���� �ݱ� ������ ������.
	// ���� �̹����� �ݱ� ������ �����ϸ� ���� �κп��� ������ �ޱ� ������ ���� �̹����� 4�� ũ�⿡ �̹����� �����ϰ� 
	// �����̹����� ����� �־� �� �κп��� �ݱ� ������ �� �ǵ��� ����.
	Mat rect_s45_2(rect_s45.rows * 2, rect_s45.cols * 2, CV_8UC1, Scalar(0)); 
	for (int x = 0; x < rect_s45.cols; x++) {
		for (int y = 0; y < rect_s45.rows; y++) {
			if (rect_s45.at<uchar>(y, x) == 255) {
				rect_s45_2.at<uchar>(y + rect_s45.rows / 2, x + rect_s45.cols / 2) = 255;
			}
		}
	}
	morphologyEx(rect_s45_2, rect_s45_2, MORPH_CLOSE, Mat(), Point(-1, -1), 6); // �ݱ� ���� ����
	//		imshow("s45_2", rect_s45_2);

	// 4�� Ű�� �����̿��� ���� ������ �ٽ� ������.
	Mat rect_s45_3 = rect_s45_2(Rect(rect_s45.cols / 2, rect_s45.rows / 2, rect_s45.cols, rect_s45.rows));
	//		imshow("s45_3", rect_s45_3); // �� �����ȿ��� hsv �м���.

	return rect_s45_3;
}
int ohm_hsv_color(Mat rect_s45_3, Mat rect_hsv) {
	vector<Mat> aaa; // �� ������ ������ ����
	for (int i = 0; i < 10; i++) {
		aaa.push_back(Mat(rect_s45_3.rows, rect_s45_3.cols, CV_8UC1, Scalar(0))); // 10���� ���͸� �����ϰ� 0���� �ʱ�ȭ ��.
	}
	for (int x = 0; x < rect_s45_3.cols; x++) {
		for (int y = 0; y < rect_s45_3.rows; y++) {
			if (rect_s45_3.at<uchar>(y, x) == 255) { // ���� ������ ��ǥ�� 255�� ��� (���� ������ ���)
				Vec3b& p = rect_hsv.at<Vec3b>(y, x);

				// �� ������ ���� ã�� ����. HSV���� �м���.
				// ���� 1
				if ((p[0] <= 5) && (p[1] >= 80 && p[1] <= 140) && (p[2] >= 60 && p[2] <= 100)) {
					int color_num = 1;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// ���� 2
				if ((p[0] <= 5 || p[0] >= 175) && (p[1] >= 140) && (p[2] >= 90)) {
					int color_num = 2;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// ��Ȳ 3
				if ((p[0] >= 8 && p[0] <= 10) && (p[1] >= 157) && (p[2] >= 100)) {
					int color_num = 3;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// ��� 4
				if ((p[0] >= 16 && p[0] <= 24) && (p[1] >= 130)) {
					int color_num = 4;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// �ʷ� 5
				if ((p[0] >= 25 && p[0] <= 80)) {
					int color_num = 5;
					aaa[color_num].at<uchar>(y, x) = 255;
				}
				// ���� 0
				else if ((p[2] <= 60)) {
					int color_num = 0;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// �Ķ� 6
				if ((p[0] >= 100 && p[0] <= 140) && (p[1] <= 120) && (p[2] <= 80)) {
					int color_num = 6;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// ���� 7
				if ((p[0] <= 8 || p[0] >= 170) && (p[1] >= 60 && p[1] <= 130) && (p[2] <= 115)) {
					int color_num = 7;
					aaa[color_num].at<uchar>(y, x) = 255;
				}

				// ȸ�� 8
				if ((p[0] <= 20) && (p[1] >= 21 && p[1] <= 96) && (p[2] >= 71 && p[2] <= 95)) {
					int color_num = 8;
				//	aaa[color_num].at<uchar>(y, x) = 255;
				}

				// ��� 9
				if ((p[1] >= 35 && p[1] <= 80) && (p[2] >= 130 && p[2] <= 180)) {
					int color_num = 9;
					aaa[color_num].at<uchar>(y, x) = 255;
				}
			}
		}
	}

	// ���� �� �������� ����� �� �������� ���̺��� �����Ͽ� �󸶳� ����Ǿ����� Ȯ����.
	Mat labels, stats, centroids;
	vector<int> i_size, i_color; // ���̺��� ������ ũ��, ����
	vector<double> i_centroid; // �����߽��� ������.
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

	// ���� ������ �̿��Ͽ� ũŰ�� ū ������ ũ��, ����, �����߽��� ������.
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

	// ũ�� ������ ���� �� ���� ū 3���� ���Ҹ� ����
	i_size.resize(3), i_color.resize(3), i_centroid.resize(3);

	// ���� �ٽ� ���������� ���� �����߽��� ��ǥ������ �ٽ� ������.
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
	
	// �����߽��� ���� ������ ������ �����ϰ� ������ ũ�⸦ �����.
	if (i_centroid[2] < rect_s45_3.cols - i_centroid[0]) {
		ohm_result = (i_color[2] * 10 + i_color[1]) * pow(10, i_color[0]);
	}
	else {
		ohm_result = (i_color[0] * 10 + i_color[1]) * pow(10, i_color[2]);
	}
//	cout << "ohm ũ�� : " << ohm_result << endl;
	return ohm_result;
}