#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <stack>
#include "Digit_recognition.h"
#include "Digit_cut.h"
#define PLUS -2000000001
#define MINUS -2000000002
#define TIMES -2000000003
#define DIVIDE -2000000004
#define PARENTHESIS_LEFT -2000000005
#define PARENTHESIS_RIGHT -2000000006
#define PO -2000000007
#define ROUTE -2000000008
#define SQUARE -2000000009
// + - X / ( )
#define overflow -2000000000
using namespace cv;
using namespace std;
using namespace calculator;
static Point pt; // 전역변수
static stack<double> ads;
void on_mouse(int event, int x, int y, int flags, void* userdata);
vector<double> blank_space(vector<double>& space);
vector<double> calc_error_handling(vector<double>& v);
int pre(int i);
void a(int i);
vector<double> fix(vector<double>& v);
double calc(vector<double>& v);
vector<double> in_clac(vector<double>& v, int& letter_cnt_v, int& letter_po_cnt_v, int& par_left_v, int& par_right_v);
vector<double> route_result(vector<int>& route_in_cnt, vector<double> v);
vector<double> square_of(vector<double>& v);
int main(void) {
	int k;

	Mat img(640, 1760, CV_8UC1, Scalar(0));
	line(img, Point(img.cols - 160, 0), Point(img.cols - 160, img.rows), Scalar(255), 2);
	line(img, Point(0, img.rows - 160), Point(img.cols, img.rows - 160), Scalar(255), 2);
	line(img, Point(img.cols - 120, img.rows - 100), Point(img.cols - 40, img.rows - 100), Scalar(255), 5);
	line(img, Point(img.cols - 120, img.rows - 60), Point(img.cols - 40, img.rows - 60), Scalar(255), 5);
	line(img, Point(img.cols - 160, img.rows - 320), Point(img.cols, img.rows - 320), Scalar(255), 2);
	line(img, Point(img.cols - 160, img.rows - 480), Point(img.cols, img.rows - 480), Scalar(255), 2);
	putText(img, "Ans", Point(img.cols - 135, img.rows - 380), FONT_HERSHEY_DUPLEX, 2, Scalar(255), 2);
	putText(img, "AC", Point(img.cols - 125, img.rows - 220), FONT_HERSHEY_DUPLEX, 2, Scalar(255), 2);
	putText(img, "integer", Point(img.cols - 140, 70), FONT_HERSHEY_DUPLEX, 1, Scalar(255), 1);
	putText(img, "part", Point(img.cols - 140, 105), FONT_HERSHEY_DUPLEX, 1, Scalar(255), 1);
	putText(img, "Calculation expression", Point(30, img.rows - 136), FONT_HERSHEY_DUPLEX, 1, Scalar(255), 1);

	namedWindow("img");
	setMouseCallback("img", on_mouse, (void*)&img);

	imshow("img", img);
	k = waitKey();
}
void on_mouse(int event, int x, int y, int flags, void* userdata) {
	Mat img = *(Mat*)userdata;
	Mat c = img(Rect(0, 0, img.cols - 160 - 1, img.rows - 160 - 1)); // 숫자 영역
	switch (event) {
	case EVENT_LBUTTONDOWN:
		pt = Point(x, y);

		if (x > img.cols - 160 && y < img.rows - 160 && y > img.rows - 320) {
			c = Scalar(0);
		}
		else if (x > img.cols - 160 && y < img.rows - 320 && y > img.rows - 480) {
			c = Scalar(0);
			if (ads.size() == 0) {
				ads.push(0);
			}
			double a = ads.top();
			if (a - int(a) == 0) {
				int ads_result = a;
				putText(img, to_string(ads_result), Point(img.cols - 1600, img.rows - 320), FONT_HERSHEY_DUPLEX, 4, Scalar(255), 4);
			}
			else {
				double ads_result = a;
				putText(img, to_string(ads_result), Point(img.cols - 1600, img.rows - 320), FONT_HERSHEY_DUPLEX, 4, Scalar(255), 4);
			}
		}
		else if (x > img.cols - 160 && y < img.rows - 480) {
			c = Scalar(0);
			if (ads.size() == 0) {
				ads.push(0);
			}
			int a = (int)ads.top();
			putText(img, to_string(a), Point(img.cols - 1600, img.rows - 320), FONT_HERSHEY_DUPLEX, 4, Scalar(255), 4);
		}
		else if (x > img.cols - 160 && y > img.rows - 160) {
			cout << "--------------------------------------" << endl << endl;
			img(Rect(0, img.rows - 135, img.cols - 160 - 10, 135)) = Scalar(0);

			morphologyEx(c, c, MORPH_CLOSE, Mat(), Point(-1, -1), 3); // 닫기 연산으로 연결

			Mat labels, stats, centroids; // 레이블링
			int cnt = connectedComponentsWithStats(c, labels, stats, centroids);

			if (cnt == 1) {
				break;
			}

			vector<int> v, aaa; // v : x좌표 값 작음 ~ x좌표 값 큼, aaa : v벡터의 값에 대응되는 레이블링 번호
			for (int num = 1; num < cnt; num++) {
				int* p = stats.ptr<int>(num);
				v.push_back(p[0]);
			}
			sort(v.begin(), v.end());
			for (int i = 0; i < cnt - 1; i++) {
				for (int num = 1; num < cnt; num++) {
					int* p = stats.ptr<int>(num);
					if (v.at(i) == p[0]) {
						aaa.push_back(num);
					}
				}
			}

			vector<double> letter;

			int route_len = 0, route_len_stat = 0, route_in_cnt = 0, route_cnt = -1;
			vector<int> v_route_in_cnt;

			int before_p1 = 0;

			String Calculation = "";

			for (int& num : aaa) {
				cout << num << "번째 문자" << endl;

				int* p = stats.ptr<int>(num);
				double* centroid = centroids.ptr<double>(num);
				//	Mat n = img(Rect(p[0], p[1], p[2], p[3])); // 객체 영역



				Mat n, n_1;
				//	img(Rect(p[0], p[1], p[2], p[3])).copyTo(n);
				img.copyTo(n_1);
				n = n_1(Rect(p[0], p[1], p[2], p[3]));

				Mat n_labels = labels(Rect(p[0], p[1], p[2], p[3]));

				cout << n.at<uchar>(0, 0) << endl;

				for (int j = 0; j < n.rows; j++) { // 객체 안에 다른 객체가 있는 경우
					for (int i = 0; i < n.cols; i++) {
						if (n_labels.at<int>(j, i) != num) {
							n.at<uchar>(j, i) = 0;
						}
					}
				}

			//	cout << n.type() << ", " << n_labels.type() << endl;
			//	cout << "x : " << n.cols << "y ; " << n.rows << endl;

				vector<vector<Point>> contours;
				findContours(n, contours, RETR_LIST, CHAIN_APPROX_SIMPLE); // 외각선 검출

				Mat ym = Cut::y_cut(n);
				double y_avg = ym.at<int>(10, 0) / 10;

				// x age
				Mat xm = Cut::x_cut(n);
				double x_avg = xm.at<int>(0, 10) / 10.0;

				Mat cm = Cut::c_cut(n_1, p, centroid); // 
				double cm_avg = (cm.at<int>(0, 0) + cm.at<int>(1, 0) + cm.at<int>(0, 1) + cm.at<int>(1, 1)) / 4.0;
				cout << "인식된 숫자 : ";

				cout << "p2 = " << p[2] << ", p[3] = " << p[3] << endl;
				if (contours.size() == 1) { // 1,2,3,5,7,+,-,x,/,(,)
					if (Digit::minus_(p)) {
						cout << "-" << endl;
						letter.push_back(MINUS);
						Calculation += "-";
					}
					else { // 1,2,3,5,7,+,x,/,(,)
						if (Digit::one(p)) {
							cout << "1" << endl;
							letter.push_back(1);
							Calculation += "1";
						}
						else if (Digit::divide_(cm)) {
							cout << "/" << endl;
							letter.push_back(DIVIDE);
							Calculation += "/";
						}
						else {
							if (Digit::times_(img, p, centroid)) { // x썻는데 5뜸
								cout << "x" << endl;
								letter.push_back(TIMES);
								Calculation += "x";
							}
							else {
								if (Digit::plus_(img, p, centroid)) {
									cout << "+" << endl;
									letter.push_back(PLUS);
									Calculation += "+";
								}
								else {
									if (p[2] < 30 && p[3] < 30) {
										cout << "." << endl;
										letter.push_back(PO);
										Calculation += ".";
									}
									else {
										if (Digit::pi(xm, x_avg, ym, y_avg)) {
											cout << "파이 임" << endl;
											if (letter.size() != 0 and letter.back() > overflow) {
												letter.push_back(TIMES);
											}
											letter.push_back(CV_PI);
											Calculation += "pi";
										}
										else {
											double y9_y0 = abs(ym.at<int>(0, 0) - ym.at<int>(9, 0)) / y_avg;
											if (Digit::seven_route(ym, y_avg, y9_y0)) {  // 인식 안됨 가끔
												cout << cm.at<int>(1, 1) << endl;
												if (Digit::route(cm)) {
													cout << "route" << endl;
													letter.push_back(ROUTE);
													route_cnt = letter.size();

													v_route_in_cnt.push_back(0);

													route_len_stat = p[0];
													route_len = p[0] + p[2];
													Calculation += "r";
												}
												else {
													cout << "7" << endl;
													letter.push_back(7);
													Calculation += "7";
												}
											}
											else {
												int l_right = 0, l_left = 0;
												for (int j = p[1] + 0.1 * p[3]; j < p[1] + 0.9 * p[3]; j++) {
													if (img.at<uchar>(j, p[0] + 0.9 * p[2]) > 0) {
														l_right++;
														break;
													}
												}
												for (int j = p[1] + 0.1 * p[3]; j < p[1] + 0.9 * p[3]; j++) {
													if (img.at<uchar>(j, p[0] + 0.1 * p[2]) > 0) {
														l_left++;
														break;
													}
												}
												if (Digit::par_left(xm, x_avg, l_right)) {
													cout << "(" << endl;
													letter.push_back(PARENTHESIS_LEFT);
													Calculation += "(";
												}
												else if (Digit::par_right(xm, x_avg, l_left)) {
													cout << ")" << endl;
													letter.push_back(PARENTHESIS_RIGHT);
													Calculation += ")";
												}
												else {
													if (Digit::three_five(ym, y_avg)) {
														Mat n_cut = img(Rect(p[0], p[1], centroid[0] - p[0], p[3]));
														vector<vector<Point>> contours_cut;
														findContours(n_cut, contours_cut, RETR_LIST, CHAIN_APPROX_SIMPLE); // 외각선 검출
														if (contours_cut.size() == 2) {
															cout << "5" << endl;
															letter.push_back(5);
															Calculation += "5";
														}
														else if (contours_cut.size() == 3) {
															cout << "3" << endl;
															letter.push_back(3);
															Calculation += "3";
														}
														else {
															cout << "인식안되서 overflow으로 처리 " << endl;
															letter.push_back(overflow);
														}
													}
													else if (Digit::two(ym, y_avg)) {
														cout << "2" << endl;
														letter.push_back(2);
														Calculation += "2";
													}
													else {
														cout << "인식안되서 overflow으로 처리 " << endl;
														cout << ".." << endl;
														letter.push_back(overflow);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				else if (contours.size() == 2) {

					if (Digit::zero(cm, cm_avg, ym, y_avg, xm, x_avg)) {
						cout << "0" << endl;
						letter.push_back(0);
						Calculation += "0";
					}
					else if (Digit::four(xm, x_avg, ym, y_avg)) {
						cout << "4" << endl;
						letter.push_back(4);
						Calculation += "4";
					}
					else if (Digit::nine(xm, x_avg)) {
						cout << "9" << endl;
					//	cout << "cm(0,1) : " << cm.at<int>(0, 1) << ", cm(1,0) : " << cm.at<int>(1, 0) << endl;
						letter.push_back(9);
						Calculation += "9";
					}
					else if (Digit::six(xm, x_avg)) {
						if (Digit::e_(cm)) {
							cout << "e" << endl;
							if (letter.size() != 0 and letter.back() != SQUARE and letter.back() > overflow) {
								letter.push_back(TIMES);
							}
							letter.push_back(2.718281828459);
							Calculation += "e";
						}
						else {
							cout << "6" << endl;
							//	cout << "cm(0,1) : " << cm.at<int>(0, 1) << ", cm(1,0) : " << cm.at<int>(1, 0) << endl;
							letter.push_back(6);
							Calculation += "6";

							cout << " 9 dkf" << endl;
							cout << xm.at<int>(0, 9) + xm.at<int>(0, 8) << " " << 2.5 * x_avg << endl;
							cout << xm.at<int>(0, 7) + xm.at<int>(0, 8) << " " << 2.5 * x_avg << endl;
						}
					}
					else {
						cout << "인식안되서 overflow으로 처리 " << endl;
						letter.push_back(overflow);
						xm.at<int>(0, 0) + xm.at<int>(0, 1) + xm.at<int>(0, 2) > 3 * x_avg;
						cout << "xm(0,0~3) :" << xm.at<int>(0, 0) << ", " << xm.at<int>(0, 1) << ", " << xm.at<int>(0, 2) << endl;
						cout << "x_avg = " << x_avg;
						cout << "cm(0,1) : " << cm.at<int>(0, 1) << ", cm(1,0) : " << cm.at<int>(1, 0) << endl;
					}
				}
				else if (contours.size() == 3) {
					cout << "8" << endl;
					letter.push_back(8);
					Calculation += "8";
				}
				else {
					cout << "인식안되서 overflow으로 처리 " << endl;
					letter.push_back(overflow);
				}
				if (p[0] > route_len_stat and p[0] + p[2] < route_len) {
					route_in_cnt++;
					v_route_in_cnt.back()++;
				}

				if (before_p1 > p[1] + p[3]) {
					letter.emplace(letter.end() - 1, SQUARE);
					Calculation.insert(Calculation.size() - 1, "^");
				}

				if (letter.back() == PO) {
					before_p1 = 0;
				}
				else {
					before_p1 = p[1];
				}

				/*
				// 바운딩 박스
				rectangle(img, Rect(p[0], p[1], p[2], p[3]), Scalar(255), 1);

				// 무게중심
				line(img, Point(p[0], centroid[1]), Point(p[0] + p[2], centroid[1]), Scalar(255), 1);
				line(img, Point(centroid[0], p[1]), Point(centroid[0], p[1] + p[3]), Scalar(255), 1);

				// xm
				for (int i = 1; i < 10; i++) {
					line(img, Point(p[0] + 0.1 * i * p[2], p[1]), Point(p[0] + 0.1 * i * p[2], p[1] + p[3]), Scalar(255), 1);
				}

				// ym
				for (int i = 1; i < 10; i++) {
					line(img, Point(p[0], p[1] + 0.1 * i * p[3]), Point(p[0] + p[2], p[1] + 0.1 * i * p[3]), Scalar(255), 1);
				}
				*/
			}


			// 루트에 괄호 추가
			cout << Calculation << endl;
			int v_k = 0;
			if (v_route_in_cnt.size() > 0) {
				for (int i = 1; i < letter.size(); i++) {
					if (letter.at(i) == ROUTE) {
						if (letter.at(i - 1) > overflow) {
							letter.emplace(letter.begin() + i, TIMES);
						}
					}
				}
				for (int i = 0; i < Calculation.size(); i++) {
					if (Calculation[i] == 'r') {
						if (i + 2 < Calculation.size() and Calculation[i + 1] == 'p') {
							Calculation.insert(i + 2 + v_route_in_cnt.at(v_k), ")");
						}
						else {
							Calculation.insert(i + 1 + v_route_in_cnt.at(v_k), ")");
						}
						Calculation.insert(i + 1, "(");
						v_k++;
					}
				}
			}
			

			// 루트랑 지수 그림
			cout << "실행전 : " << Calculation << endl;
			vector<double> text_calc = letter;
		//	line(img, Point(30, 400), Point(30, 480), Scalar(255));
			int x_line = 30, line_po_cnt = 0;
			for (int i = 0; i < Calculation.size(); i++) {
				char ca = Calculation[i];

				if (Calculation.at(i) == '+' || Calculation.at(i) == '-') {
					x_line += 75;
				}
				else if (Calculation.at(i) == 'x') {
					x_line += 55;
				}
				else if (Calculation.at(i) == '/') {
					x_line += 70;
				}
				else if (Calculation.at(i) == '(' || Calculation.at(i) == ')') {
					x_line += 42;
				}
				else if (Calculation.at(i) == '.') {
					x_line += 32;
					if (line_po_cnt++ % 2 == 0) {
						x_line++;
					}
				}
				else if (Calculation.at(i) == 'r') {
					line(img, Point(10 + x_line, img.rows - 60), Point(20 + x_line, img.rows - 60), Scalar(255), 3);
					line(img, Point(20 + x_line, img.rows - 60), Point(30 + x_line, img.rows - 30), Scalar(255), 3);
					line(img, Point(30 + x_line, img.rows - 30), Point(45 + x_line, img.rows - 90), Scalar(255), 3);
					x_line += 50;
					Calculation.at(i) = ' ';
				}
				else if (Calculation.at(i) == '^') {
					
				//	cout << "^ 확인" << endl;
					String sq = "";
					for (int j = i + 1; j < Calculation.size();j++) {
						sq += Calculation[j];
					//	cout << "지수 : " << sq << endl;
						if (Calculation.at(j) == '+' || Calculation.at(j) == '-' || Calculation.at(j) == 'x' || Calculation.at(j) == '/' || Calculation.at(j) == ' '
							|| Calculation.at(j) == '(' || Calculation.at(j) == ')' || Calculation.at(j) == 'r' || Calculation.size() - 1 == j) {
							if (Calculation.size() - 1 != j) {
							//	cout << "sq 삭제 전 : " << sq << endl;
								sq.erase(sq.size() - 1, 1);
							}
							else {
								Calculation.erase(Calculation.size() - 1, 1);
							}
							putText(img, sq, Point(x_line - 10, img.rows - 100), FONT_HERSHEY_DUPLEX, 1, Scalar(255), 1);
							Calculation.erase(i, j - i);
							i--;
							break;
						}
					}
				}
				else {
					x_line += 60;
				}
				
			//	cout << "이번 숫자 " << Calculation.at(i) << endl;
			//	line(img, Point(x_line, 340), Point(x_line, 480), Scalar(255));
			}
			cout << "실행후 : " << Calculation << endl;




			cout << "처음 " << endl;
			for (int i = 0; i < letter.size(); i++) {
				cout << letter.at(i) << " ";
			}
			cout << endl;

			
			letter = route_result(v_route_in_cnt, letter); // 루트 값 먼저 계산
			letter = blank_space(letter); // 계산하고 남은 칸 지움

			cout << "루트 후 " << endl;
			for (int i = 0; i < letter.size(); i++) {
				cout << letter.at(i) << " ";
			}
			cout << endl;

			int letter_cnt = 0;
			int letter_po_cnt = 0;
			int par_left = 0, par_right = 0;


			letter = in_clac(letter, letter_cnt, letter_po_cnt, par_left, par_right); // 1 0 0 -> 100으로 만듦
		
			
			cout << "에러처리 시작 " << endl;
			letter = calc_error_handling(letter);
			cout << "calc_error_handling 실행 후 값 : ";


			for (int i = 0; i < letter.size(); i++) {
				cout << letter.at(i) << " ";
			}
			cout << endl;
			// 빈칸(overflow) 제거
			letter = blank_space(letter);
			cout << "in_clac 실행 후 : ";
			for (int i = 0; i < letter.size(); i++) {
				cout << letter.at(i) << " ";
			}
			cout << endl;

			letter = square_of(letter); // 지수 계산

			


			// 빈칸(overflow) 제거
			letter = blank_space(letter);
			
			//  + x -> + 0 x 
			int a = 0;
			for (int i = 1; i < letter.size(); i++) {
				if (letter.at(i) == PLUS || letter.at(i) == MINUS) {
					a = 1;
				}
				else if (a == 1 && (letter.at(i) == TIMES || letter.at(i) == DIVIDE)) {
					letter.emplace(letter.begin() + i, 0);
					a = 0;
				}
				else {
					a = 0;
				}

			}
			double res;
			if (letter.size() > 0) {
				letter = fix(letter);
				res = calc(letter); // 소수 7째 자리에서 반올림

				Calculation += "=";

				if (round(res * 1000000) / 1000000 - (int)res == 0) {
					int result = (int)res;
					Calculation += to_string(result);
					ads.push(result);
				}
				else {
					double result = res;
					Calculation += to_string(result);
					ads.push(result);
				}
			}
			cout << Calculation << endl;
			putText(img, Calculation, Point(30, img.rows - 30), FONT_HERSHEY_DUPLEX, 3, Scalar(255), 2);
		}
		cout << ads.size();
		imshow("img", img);
		break;

	case EVENT_MOUSEMOVE:
		if (flags & EVENT_FLAG_LBUTTON) {
			if (x < img.cols - 160 - 1 && y < img.rows - 160 - 1)
				line(img, pt, Point(x, y), Scalar(255), 5);
			imshow("img", img);
			pt = Point(x, y);
		}
		break;
	}
}

// 빈칸(overflow) 제거
vector<double> blank_space(vector<double>& space) {
	vector<double> letter = space;
	for (vector<double>::iterator it = letter.begin(); it != letter.end();) {
		if (*it == overflow) {
			it = letter.erase(it);
		}
		else {
			it++;
		}
	}
	return letter;
}
vector<double> calc_error_handling(vector<double>& v) {
	vector<double> letter = v;

	int calc_cnt = 0;
	for (int i = 0; i < letter.size(); i++) {
		if (letter.at(i) <= PLUS && letter.at(i) >= DIVIDE) {
			if (calc_cnt > 0) {
				letter.at(i) = overflow;
			}
			calc_cnt++;
		}
		else {
			calc_cnt = 0;
		}
	}
	blank_space(letter);
	cout << "e" << endl;
	int cnt_num = 0;
	for (int i = 0; i < letter.size(); i++) {
		if (letter.at(i) > overflow) {
			cnt_num++;
			break;
		}
	}
	cout << "cnt_num : " << cnt_num << endl;
	if (cnt_num == 0) {
		letter.clear();
		letter.push_back(0);
	}

	for (int i = 0; i < letter.size(); i++) {
		cout << letter.at(i) << " ";
	}

	// 1. 기호 1개만 입력된 경우 : ( , ) , x ...
	if (letter.size() == 1 && letter.at(0) <= overflow) { // ) ㅊ
		letter.at(0) = 0; // 그 값을 0으로 변경.
	}
	// 2. 괄호 안에 아무 값도 없는 경우 : ()
	else {
		for (int i = 0; i < letter.size(); i++) {
			if (letter.at(i) == PARENTHESIS_LEFT) {
				// ( 로 식이 끝난 경우.
				// 2 + 3 - 2 ( 

				if (letter.size() - 1 == i) {
					letter.at(i) = overflow;
					// 2 + 3 - 2 x ( 
					
					if (i != 0 && letter.at(i - 1) < overflow && letter.at(i - 1) > PARENTHESIS_LEFT) {
						letter.at(i - 1) = overflow;
					}
					blank_space(letter);
				}
				cout << "l ";
				for (int i = 0; i < letter.size(); i++) {
					cout << letter.at(i) << " ";
				}
				cout << endl;

				// () -> (0)
				if ((i + 1) < letter.size()) {
					if (letter.at(i + 1) == PARENTHESIS_RIGHT) {
						letter.emplace(letter.begin() + i + 1, 0);
					}

					// ( + 2 ) -> ( 0 + 2 )
					else if (letter.at(i + 1) == PLUS || letter.at(i + 1) == MINUS) {
						letter.emplace(letter.begin() + i + 1, 0);
					}
				}

				cout << "e ";
				for (int i = 0; i < letter.size(); i++) {
					cout << letter.at(i) << " ";
				}
				cout << endl;

				if (i > 0 and letter.at(i - 1) > overflow) {
					if (letter.size() - 1 == i) {
						letter.at(i) = overflow;
						blank_space(letter);
					}
					else {
						letter.emplace(letter.begin() + i, TIMES);
					}
				}
			}
		}
	}
	blank_space(letter);
	cout << "a" << endl;
	
	for (int i = 0; i < letter.size(); i++) {
		cout << letter.at(i) << " ";
	}
	cout << endl;
	

	int z = 0;
	int z_before = 0;
	while (z < letter.size()) {
		double z_num = letter.at(z);
		if (z_num == PLUS || z_num == MINUS) {
			if (z == 0) { // + a -> a, - a -> -a
				letter.emplace(letter.begin(), 0);
			}
			else if (z > 0 and (letter.at(z_before) == PLUS || letter.at(z_before) == MINUS ||
				letter.at(z_before) == TIMES || letter.at(z_before) == DIVIDE)) {
				if (z_num == PLUS) { // a x + 2 -> a x 2
					letter.at(z) = letter.at(z + 1);
					letter.erase(letter.begin() + z + 1);
				}
				else { // a / - 3 -> a / -3
					letter.at(z) = -1 * letter.at(z + 1);
					letter.erase(letter.begin() + z + 1);
				}
			}
		}
		z_before = z;
		z++;
	}
	blank_space(letter);
	cout << "b" << endl;
	if (letter.size() > 0) {
		if (letter.at(0) == TIMES || letter.at(0) == DIVIDE) { // x 2 -> 0 x 2
			letter.emplace(letter.begin(), 0);
		}

		if (letter.back() == PLUS || letter.back() == MINUS || letter.back() == TIMES || letter.back() == DIVIDE) {
			letter.push_back(0); // 2 + -> 2 + 0
		}
	}
	cout << "c" << endl;
	int par_left = 0, par_right = 0;
	for (int i = 0; i < letter.size(); i++) {
		if (letter.at(i) == PARENTHESIS_LEFT) {
			par_left++;
		}
		else if (letter.at(i) == PARENTHESIS_RIGHT) {
			par_right++;
		}
	}

	if (par_left > par_right) { // (( ) -> (( ))
		for (int i = par_right; i < par_left; i++) {
			letter.push_back(PARENTHESIS_RIGHT);
		}
	}
	else if (par_left < par_right) { // ( )) -> (( ))
		for (int i = par_left; i < par_right; i++) {
			letter.emplace(letter.begin(), PARENTHESIS_LEFT);
		}
	}
	cout << "d" << endl;
	
	return letter;
}
int pre(int i) {
	switch (i) {
	case PARENTHESIS_LEFT: case PARENTHESIS_RIGHT: return 0;

	case PLUS: case MINUS: return 1;

	case TIMES: case DIVIDE: return 2;
	}
	return -1;
}
void a(int i) {
	switch (i) {
	case PLUS: cout << "+";
		break;
	case MINUS: cout << "-";
		break;
	case TIMES: cout << 'x';
		break;
	case DIVIDE: cout << "/";
		break;
	case PARENTHESIS_LEFT: cout << "(";
		break;
	case PARENTHESIS_RIGHT: cout << ")";
		break;
	case PO :cout << ".";
		break;
	case ROUTE: cout << "route";
		break;
	case SQUARE: cout << "^";
		break;
	}
	cout << " ";
}
vector<double> fix(vector<double>& v) {
	double c, op;
	stack<int> st;
	int k = 0;

	vector<double> result;

	while (k < v.size()) {
		c = v.at(k);
		if (c == PARENTHESIS_LEFT) st.push(c);
		else if (c == PARENTHESIS_RIGHT) {
			while (!st.empty()) {
				op = st.top();
				st.pop();
				if (op == PARENTHESIS_LEFT) break;
				else {
					a(op);
					result.push_back(op);
				}
			}
		}
		else if (c == PLUS || c == MINUS || c == TIMES || c == DIVIDE) {
			while (!st.empty()) {
				op = st.top();
				if (pre(c) <= pre(op)) {
					a(op);
					result.push_back(op);
					st.pop();
				}
				else break;
			}
			st.push(c);
		}
		else {
			cout << c << " ";
			result.push_back(c);
		}
		k++;
	}
	while (!st.empty()) {
		a(st.top());
		result.push_back(st.top());
		st.pop();
	}
	return result;
}
double calc(vector<double>& v) {
	stack<double> st;
	double c;
	int k = 0;
	while (k < v.size()) {
		c = v.at(k);
		if (c == PLUS || c == MINUS || c == TIMES || c == DIVIDE) {
			double val2 = st.top();
			st.pop();
			double val1 = st.top();
			st.pop();
			switch ((int)c) {
			case PLUS: st.push(val1 + val2); break;
			case MINUS: st.push(val1 - val2); break;
			case TIMES: st.push(val1 * val2); break;
			case DIVIDE: st.push(val1 / val2); break;
			}
		}
		else {
			st.push(c);
		}
		k++;
	}
	c = st.top();
	return c;
}
vector<double> in_clac(vector<double>& v, int& letter_cnt_v, int& letter_po_cnt_v, int& par_left_v, int& par_right_v) {
	vector<double> letter = v;
	int letter_cnt = 0, letter_po_cnt = 0, par_left = 0, par_right = 0;

	cout << "in_clac에 들어온 값 " << endl;
	for (int i = 0; i < letter.size(); i++) {
		cout << letter.at(i) << " ";
	}
	cout << endl;



	for (int i = 0; i < letter.size(); i++) {
		if (letter.at(i) > overflow) {
			int j = i - letter_cnt;
			if (letter_cnt > 0 && letter_po_cnt == 0) {
				letter.at(j) = letter.at(j) * 10 + letter.at(i);
				letter.at(i) = overflow;
			}
			else if (letter_po_cnt > 0) {
				if (letter_cnt == 1) {
					letter.at(j) = 0;
				}
				letter.at(j) = letter.at(j) + pow(0.1, letter_po_cnt) * letter.at(i);
				letter.at(i) = overflow;
				letter_po_cnt++;
			}
			letter_cnt++;
		}
		else {
			if (letter.at(i) == PO) {
				letter_po_cnt++;
				letter_cnt++;
				letter.at(i) = overflow;
			}
			else if (letter.at(i) == PARENTHESIS_LEFT) {
				par_left++;
				letter_cnt = 0;
				letter_po_cnt = 0;
			}
			else if (letter.at(i) == PARENTHESIS_RIGHT) {
				par_right++;
				letter_cnt = 0;
				letter_po_cnt = 0;
			}
			else {
				letter_cnt = 0;
				letter_po_cnt = 0;
			}
		}
	}

	cout << "in_clac에서 나가는 값 " << endl;
	for (int i = 0; i < letter.size(); i++) {
		cout << letter.at(i) << " ";
	}
	cout << endl;


	letter_cnt_v = letter_cnt, letter_po_cnt_v = letter_po_cnt;
	par_left_v = par_left, par_right_v = par_right;
	return letter;
}
vector<double> route_result(vector<int>& route_in_cnt, vector<double> v) {
	vector<double> letter = v;
	vector<double> c;
	int a = 0, b = 0, e = 0, d = 0;
	double res;
	vector<int> i_route_stat;
	vector<double> d_calc;
	for (int i = 0; i < letter.size(); i++) {
		if (letter.at(i) == ROUTE) {
			i_route_stat.push_back(i);
		}
	}
	for (int i = 0; i < i_route_stat.size(); i++) {
		if (route_in_cnt.at(i) == 0) {
			d_calc.push_back(0);
		}
		for (int j = 0; j < route_in_cnt.at(i); j++) {
			d_calc.push_back(letter.at(i_route_stat.at(i) + j + 1));
			letter.at(i_route_stat.at(i) + j + 1) = overflow;
		}
		cout << "a";
		d_calc = in_clac(d_calc, a, b, e, d);
		cout << "b";
		d_calc = blank_space(d_calc);
		d_calc = calc_error_handling(d_calc);
		cout << "c";
		d_calc = fix(d_calc);
		cout << "d";
		res = sqrt(calc(d_calc));
		cout << "e";
		letter.at(i_route_stat.at(i)) = res;
		cout << "f";
		d_calc.clear();
		cout << "g";
	}

	cout << "결과 : ";
	for (int i = 0; i < letter.size(); i++) {
		cout << letter.at(i) << " ";
	}
	cout << endl;

	cout << "route_reust 함수 종료 " << endl;
	return letter;
}
vector<double> square_of(vector<double>& v) {
	vector<double> letter = v;
	cout << "지수 계산 실행 전" << endl;
	for (int i = 0; i < letter.size(); i++) {
		if (letter.at(i) < overflow) {
			a(letter.at(i));
		}
		else if (letter.at(i) == overflow) {
			cout << "overflow ";
		}
		else {
			cout << letter.at(i) << " ";
		}
	}
	cout << endl;
	int square_cnt = 0;
	for (int i = 0; i < letter.size(); i++) {
		if (letter.at(i) == SQUARE) {
			square_cnt++;
		}
	}
	if (square_cnt == 0) {
		return letter;
	}
	int k = 0;
	int num = -1;

	int par_left = -1;

	// 괄호 없는거
	for (int i = 0; i < letter.size(); i++) {
		if (k == 0 && letter.at(i) > overflow) {
			num = i;
		}
		else if (k == 0 && letter.at(i) == PARENTHESIS_RIGHT) {
			num = i;
		}
		else if (num > -1 and letter.at(i) == SQUARE) {
			k = i;
			letter.at(i) = overflow;
		}
		else if (num > -1 and k) {
			if (letter.at(num) != PARENTHESIS_RIGHT) {
				k = 0;
				letter.at(num) = pow(letter.at(num), letter.at(i));
				letter.at(i) = overflow;
				cout << "3";
			}
			else { // 안됨
				cout << " zz" << endl;
				int par_left;
				vector<double> par_square;
				int sq_res = letter.at(i);

				letter.at(i) = overflow;
				for (int j = num; j >= 0; j--) {
					if (letter.at(j) == PARENTHESIS_LEFT) {
						par_left = j;
					}
				}

				cout << endl;
				cout << "par_left : " << par_left << ", num : " << num << endl;
				for (int j = par_left + 1; j < num; j++) {
					par_square.push_back(letter.at(j));
					letter.at(j) = overflow;
				}

				cout << "k3 ";
				for (int j = 0; j < letter.size(); j++) {
					if (letter.at(j) < overflow) {
						a(letter.at(j));
					}
					else if (letter.at(j) == overflow) {
						cout << "overflow ";
					}
					else {
						cout << letter.at(j) << " ";
					}
				}
				cout << endl;

				cout << "par : ";
				for (int j = 0; j < par_square.size(); j++) {
					if (par_square.at(j) < overflow) {
						a(par_square.at(j));
					}
					else if (par_square.at(j) == overflow) {
						cout << "overflow ";
					}
					else {
						cout << par_square.at(j) << " ";
					}
				}
				cout << endl << "letter : ";
				for (int j = 0; j < letter.size(); j++) {
					if (letter.at(j) < overflow) {
						a(letter.at(j));
					}
					else if (letter.at(j) == overflow) {
						cout << "overflow ";
					}
					else {
						cout << letter.at(j) << " ";
					}
				}
				cout << endl;
				letter.at(num) = overflow;
				cout << endl;
				par_square = fix(par_square);
				
				letter.at(par_left) = pow(calc(par_square), sq_res);
				letter = blank_space(letter);
				
			}
		}
	}

	letter = blank_space(letter);
	cout << "지수 함수 실행 결과" << endl;
	for (int i = 0; i < letter.size(); i++) {
		if (letter.at(i) < overflow) {
			a((int)letter.at(i));
		}
		else if (letter.at(i) == overflow) {
			cout << "overflow ";
		}
		else {
			cout << letter.at(i) << " ";
		}
	}
	
	cout << endl;

	return letter;
}