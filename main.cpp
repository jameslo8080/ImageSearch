/**
* CS4185 Multimedia Technologies and Applications
* Course Assignment
* Image Retrieval Project
*/

#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <map>
#include <algorithm>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "feature_loader.h"
#include "feature_extract.h"
#include "feature_preprocess.h"

#include "ml.h"
#include "compare.h"
#include "method_report.h"

using namespace std;
using namespace cv;

#define IMAGE_LIST_FILE "inputimage.txt"  // NOTE: this is relative to current file

/*
man			: 0 ~ 99, 100?
beach		: 101 ~ 199, 100?
building	: 200 ~ 299
bus			: 300 ~ 399
dinosaur	: 400 ~ 499
elephant	: 500 ~ 599
flower		: 600 ~ 699
horse		: 700 ~ 799
mountain	: 800 ~ 899
food		: 900 ~ 999
*/

// not supported: 5 
string files[] = { "man", "beach", "building", "bus", "dinosaur", "elephant", "flower", "horse", "mountain", "food" };
int valid_indexs[] = { 0, 1, 2, 3, 4, 6, 7 };

string getFilePath(int index) {
		if (index < 0 || index >= 10)
				index = 0;
		string s = "./" + files[index] + ".jpg";
		return s;
}

void file_select_instruction() {
		cout << "Which file? (0:'man' 1:'beach', 2:'building', 3:'bus', 4:'dinosaur', " << endl
				<< "            6:'flower', 7:'horse')? " << endl;
}

// "../image.orig/685.jpg"
string getFilePath999(int index) {
		if (index < 0 || index > 999)
				index = 0;
		string s("../image.orig/");
		s.append(std::to_string(index) + ".jpg");
		return s;
}

Mat get_input_img(int index) {
		string filepath = getFilePath(index);

		Mat src_input = imread(filepath); // read input image
		if (!src_input.data) {
				printf("Cannot find the input image!\n");
				exit(1);
		}
		return src_input;
}

void waitESC() {
		int keyValue = 0;
		while (keyValue >= 0) {
				keyValue = cvWaitKey(0);

				switch (keyValue) {
						case 27:keyValue = -1;
								break;
				}
		}
}

double solve(int index) {
		Mat src_input = get_input_img(index);
		double acc = 0;

		//imshow("Input", src_input);
		// double acc = hsv_compare(src_input, index);
		// double acc = hsv_split_compare(src_input, index);
		// double acc = surf_compare(src_input, index);
		// double acc = sift_compare(src_input, index);
		// double acc = orb_compare(src_input, index);
		// double acc = psnr_compare(src_input, index);
		// double acc = mssim_compare(src_input, index);

		// combin_compare_6
		// double acc = combin_compare_16(src_input, index, 1.2);

		return acc;
}

void test_combin() {
		double besti = -1, maxac = 0;

		int count = 0;
		MethodResult best_mr;
		for (double con_setting = 1.15; con_setting <= 1.25; con_setting += 0.05) {
				printf("\n -- i = %lf -- \n", con_setting);

				vector<double> acc_;

				for (int i = 0; i < 7; i++) {
						acc_.push_back(0);
				}

				MethodResult mr;
				mr.add_double_setting(con_setting);

				// for each input
				for (auto index : valid_indexs) {
						Mat src_input = get_input_img(index);
						//acc += combin_compare_15(src_input, index, i);
						if (count == 0) {
								//acc_[0] += combin_compare_1(src_input, index);
								//acc_[1] += combin_compare_2(src_input, index);
								//acc_[2] += combin_compare_3(src_input, index);
								//acc_[3] += combin_compare_4(src_input, index);
						} else {
								acc_[0] += 0;
								acc_[1] += 0;
								acc_[2] += 0;
								acc_[3] += 0;
						}

						acc_[4] += combin_compare_5(src_input, index, con_setting);
						acc_[5] += 0;
						//acc_[6] += combin_compare_7(src_input, index, con_setting);


				}
				for (int i = 0; i < 7; i++) {
						acc_[i] /= 7;
						mr.add_acc(acc_[i]);
				}

				double mr_best_acc = mr.getBestAcc();
				if (mr_best_acc > maxac) {
						maxac = mr_best_acc;
						besti = con_setting;
						best_mr = mr;
				}
				printf("\n so far: Best acc: %lf || i = %lf\n", maxac, besti);
				best_mr.report();
		}
		printf("\nBest acc: %lf || i = %lf\n", maxac, besti);
		best_mr.report();
}

void test_all_average() {
		double cnt = 7;
		double totacc = 0;
		vector<double> accs;
		for (auto idx : valid_indexs) {
				double acc = solve(idx);
				totacc += acc;
				accs.push_back(acc);
		}

		for (int i = 0; i < accs.size(); ++i) {
				int idx = valid_indexs[i];
				double acc = accs[i];
				cout << "Case " << idx << " acc: " << acc << endl;
		}
		cout << "Total average acc: " << totacc / cnt << endl;
}

void test_r1c1_to_r10c10() {
		for (auto v_index : valid_indexs) {
				Mat src_input = get_input_img(v_index);

				printf("Work for img \"%s\" :\n", files[v_index].c_str());
				for (int i = 0; i < 11; i++)
						for (int j = 0; j < 11; j++)
								hsv_split_compare(src_input, v_index, false, i + 1, j + 1);

				printf("Done for img \"%s\".\n", files[v_index].c_str());

		}
}

void test_all_hsv_compare() {
		for (auto v_index : valid_indexs) {
				Mat src_input = get_input_img(v_index);

				printf("Work for img \"%s\" :\n", files[v_index].c_str());
				hsv_compare(src_input, v_index);
				printf("Done for img \"%s\".\n", files[v_index].c_str());

		}
}


int main(int argc, char** argv) {
		Mat src_input;

		Mat max_img;

		file_select_instruction();
		cout << " or -1 for all average: " << endl;
		cout << " or -2 for preprocessing(): " << endl;
		cout << " or -3 for test from r1c1 to r10c10  : " << endl;
		cout << " or -4 for test all hsv_compare()    : " << endl;
		cout << " or -5 for double_compare()          : " << endl;
		cout << " or -6 for test_combin()             : " << endl;

		int index;
		cin >> index;

		if (index == -2) {
				bool greyscale;
				cout << "0 not greyScale, 1 greyScale: \n";
				cin >> greyscale;

				int type;
				cout << " 0 for sift , 1 for surf, 2 for orb, 3 for sift bow, 4 for surf bow, 5 for orb bow: " << endl;
				cin >> type;
				if (type == 0)
						save_allSIFTDescriptions_YML(greyscale);
				else if (type == 1)
						save_allSURFDescriptions_YML(greyscale);
				else if (type == 2)
						save_allORBDescriptions_YML(greyscale);
				else if (type == 3)
						save_BOW("SIFT", greyscale);
				else if (type == 4)
						save_BOW("SURF", greyscale);
				else if (type == 5)
						save_BOW("ORB", greyscale);

				waitESC();
				return 0;
		} else if (index == -1) {
				test_all_average();
		} else if (index == -3) {
				test_r1c1_to_r10c10();
		} else if (index == -4) {
				test_all_hsv_compare();
		} else if (index == -5) {
				double_compare();
		} else if (index == -6) {
				test_combin();
		} else {
				solve(index);
		}

		do {
				cout << "input -1 for exit...\n";
				cin >> index;
		} while (index != -1);

		waitESC();

		return 0;
}
