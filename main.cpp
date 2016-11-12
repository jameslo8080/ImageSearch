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

bool isvalidindex(int k) {
	for (int i = 0; i < 7; ++i)
		if (valid_indexs[i] == k) return true;
	return false;
}

string getFilePath(int index) {
	if (index < 0 || index >= 10)
		index = 0;
	string s = "./" + files[index] + ".jpg";
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

void solve() {
	cout << "==============================================================" << endl;
	cout << "==                     Core  function                       ==" << endl;
	cout << "==============================================================" << endl;

	int index = -1;
	cout << "Which image? (0:'man' 1:'beach', 2:'building', 3:'bus', 4:'dinosaur', " << endl
		<< "            6:'flower', 7:'horse')? " << endl
		<< "Input the id to select" << endl;
	cout << "--------------------------------------------------------------" << endl;

	while (true) {
		cin >> index;
		if (isvalidindex(index)) break;
		else cout << "Invalid input, input again" << endl;
	}

	double threshold;
	cout << "Input the threshold: (the recommended threshold is 3.628534)" << endl; //3.628534 , 1.287449
	cout << "--------------------------------------------------------------" << endl;
	cin >> threshold;

	Mat src_input = get_input_img(index);
	vector<int> res = new_compare(src_input, index, threshold);
	if (res.size() > 0) {
		save_result(res, index);

		cout << "Done! The similiar images are saved in folder ../output." << endl;

		Mat mostSimiliar = imread(getFilePath999(res[0]));
		cout << "Here are the most similiar image\n" << endl;
		imshow("The most similiar image", mostSimiliar);
		waitESC();
	} else {
		cout << "the threshold maybe too high." << endl;
	}

}

void extra_fucntion() {
	cout << "==============================================================" << endl;
	cout << "==                     Extra function                       ==" << endl;
	cout << "==============================================================" << endl;
	cout << "1.  pixel-by-pixel compare" << endl;
	cout << "2.  hsv histrogram compare" << endl;
	cout << "3.  splited image + hsv histrogram compare" << endl;
	cout << "4.  SURF compare" << endl;
	cout << "5.  SIFT compare" << endl;
	cout << "6.  ORB compare" << endl;
	cout << "7.  PSNR compare" << endl;
	cout << "8.  MSSIM compare" << endl;
	cout << "9.  SVM compare (require the manually label for the dataset)" << endl;

	cout << "10. pixel-by-pixel compare + hsv historgram compare(correlation)" << endl;
	cout << "11. equalized greyscale historgram compare(correlation)" << endl;
	cout << "12.  equalized greyscale historgram + hsv histrogram compare(correlation)" << endl;
	cout << "13. equalized greyscale historgram(correlation) + PSNR compare" << endl;
	cout << "14. high contrast + hsv histrogram compare(correlation)" << endl;
	cout << "15. high contrast + splited image + hsv histrogram compare(correlation)" << endl;
	cout << "16. high contrast + PSNR" << endl;
	cout << "17. high contrast + splited image + PSNR" << endl;
	cout << "18. high contrast + MSSIM" << endl;
	cout << "19. splited image + equalized greyscale historgram compare(correlation)" << endl;
	cout << "20. high contrast + rgb histrogram compare(correlation)" << endl;
	cout << "21. high contrast + rgb histrogram compare(correlation) + circle mask" << endl;
	cout << "22. high contrast + hsv histrogram compare(correlation) + circle mask" << endl;
	cout << "23. equalized greyscale histrogram compare(correlation) + circle mask" << endl;
	cout << "24. hsv histrogram compare(correlation) + circle mask + PSNR" << endl;
	cout << "25. high contrast + hsv histrogram compare(correlation) + SURF" << endl;
	cout << "26. high contrast + hsv histrogram compare(correlation) + SIFT" << endl;
	cout << "27. high contrast + hsv histrogram compare(correlation) + ORB" << endl;
	cout << "28. Canny + contour compare" << endl;
	cout << "--------------------------------------------------------------" << endl;

	int option = -1;
	cin >> option;
	while ((option < 1 || option > 27) && option != 8888 && option != 9999) {
		cout << "Invalid input, input again" << endl;
		cin >> option;
	}
	switch (option) {
		case 1:
			for (auto idx : valid_indexs) pixel_compare(get_input_img(idx), idx);
			break;
		case 2:
			for (auto idx : valid_indexs) hsv_compare(get_input_img(idx), idx);
			break;
		case 3:
			for (auto idx : valid_indexs) hsv_split_compare(get_input_img(idx), idx);
			break;
		case 4:
			for (auto idx : valid_indexs) surf_compare(get_input_img(idx), idx);
			break;
		case 5:
			for (auto idx : valid_indexs) sift_compare(get_input_img(idx), idx);
			break;
		case 6:
			for (auto idx : valid_indexs) orb_compare(get_input_img(idx), idx);
			break;
		case 7:
			for (auto idx : valid_indexs) psnr_compare(get_input_img(idx), idx);
			break;
		case 8:
			for (auto idx : valid_indexs) mssim_compare(get_input_img(idx), idx);
			break;
		case 9:
			for (auto idx : valid_indexs) svm_compare(get_input_img(idx), idx);
			break;
		case 10:
			for (auto idx : valid_indexs) combin_compare_1(get_input_img(idx), idx);
			break;
		case 11:
			for (auto idx : valid_indexs) combin_compare_2(get_input_img(idx), idx);
			break;
		case 12:
			for (auto idx : valid_indexs) combin_compare_3(get_input_img(idx), idx);
			break;
		case 13:
			for (auto idx : valid_indexs) combin_compare_4(get_input_img(idx), idx);
			break;
		case 14:
			for (auto idx : valid_indexs) combin_compare_5(get_input_img(idx), idx);
			break;
		case 15:
			for (auto idx : valid_indexs) combin_compare_6(get_input_img(idx), idx);
			break;
		case 16:
			for (auto idx : valid_indexs) combin_compare_7(get_input_img(idx), idx);
			break;
		case 17:
			for (auto idx : valid_indexs) combin_compare_7a(get_input_img(idx), idx);
			break;
		case 18:
			for (auto idx : valid_indexs) combin_compare_8(get_input_img(idx), idx);
			break;
		case 19:
			for (auto idx : valid_indexs) combin_compare_9(get_input_img(idx), idx);
			break;
		case 20:
			for (auto idx : valid_indexs) combin_compare_10(get_input_img(idx), idx);
			break;
		case 21:
			for (auto idx : valid_indexs) combin_compare_11(get_input_img(idx), idx);
			break;
		case 22:
			for (auto idx : valid_indexs) combin_compare_12(get_input_img(idx), idx);
			break;
		case 23:
			for (auto idx : valid_indexs) combin_compare_13(get_input_img(idx), idx);
			break;
		case 24:
			for (auto idx : valid_indexs) combin_compare_14(get_input_img(idx), idx);
			break;
		case 25:
			for (auto idx : valid_indexs) combin_compare_15(get_input_img(idx), idx);
			break;
		case 26:
			for (auto idx : valid_indexs) combin_compare_16(get_input_img(idx), idx);
			break;
		case 27:
			for (auto idx : valid_indexs) combin_compare_17(get_input_img(idx), idx);
			break;
		case 28:
			for (auto idx : valid_indexs) combin_compare_18(get_input_img(idx), idx);
			break;
		case 8888:
			double_compare(); break;
		case 9999:
			test_read(); break;
		default:
			break;
	}

	cout << "Done" << endl;
}


int main(int argc, char** argv) {
	Mat src_input;

	Mat max_img;

	int instruction;
	cout << "==============================================================" << endl;
	cout << "== Input the number before the option to select that option ==" << endl;
	cout << "==============================================================" << endl;
	cout << "0. The core function: (Searching for similiar image)" << endl;
	cout << "1. Extra function: (Testing the accuracy of different image retrieval function)" << endl;
	cout << "--------------------------------------------------------------" << endl;

	cin >> instruction;
	while (instruction != 0 && instruction != 1) {
		cin >> instruction;
		cout << "Invalid input, input again" << endl;
	}

	if (instruction == 0) {
		solve();
	} else {
		extra_fucntion();
	}

	return 0;
}
