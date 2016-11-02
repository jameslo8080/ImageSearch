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
#include "feature_compare.h"

#include "ml.h"
#include "compare.h"

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

string getFilePath(int index){
	if (index < 0 || index >= sizeof(files))
		index = 0;
	string s("./");
	s.append(files[index]).append(".jpg");
	return s;
}

void file_select_instruction() {
	cout << "Which file? (0:'man' 1:'beach', 2:'building', 3:'bus', 4:'dinosaur', " << endl
		<< "            6:'flower', 7:'horse')? " << endl;
}

string askFile(){
	file_select_instruction();
	int index;
	cin >> index;
	while (index <-2 || index == 5 || index > 7)
	{
		file_select_instruction();
		cin >> index;
	}
	return getFilePath(index);
}

// "../image.orig/685.jpg"
string getFilePath999(int index){
	if (index < 0 || index > 999)
		index = 0;
	string s("../image.orig/");
	s.append(std::to_string(index) + ".jpg");
	return s;
}

void waitESC() {
	// Wait for the user to press a key in the GUI window.
	//Press ESC to quit
	int keyValue = 0;
	while (keyValue >= 0)
	{
		keyValue = cvWaitKey(0);

		switch (keyValue)
		{
		case 27:keyValue = -1;
			break;
		}
	}
}

double solve(int index) {
	string filepath = getFilePath(index);

	Mat src_input = imread(filepath); // read input image
	if (!src_input.data)
	{
		printf("Cannot find the input image!\n");
		exit(1);
	}

	//imshow("Input", src_input);
	double acc = hsv_compare(src_input, index);
	// double acc = hsv_split_compare(src_input, index);
	// double acc = surf_compare(src_input, index);
	// double acc = svm_compare(src_input, index);
	return acc;
}

double solve(Mat src_input, vector<Mat> features, int index) {
	hsv_compare(src_input, features, index);
	return 0;
}

double solve(Mat src_input, vector<Mat> imgs, int src_index, int row, int col) {
	hsv_split_compare(src_input, imgs, src_index, false, row, col);
	return 0;
}

int main(int argc, char** argv){
	Mat src_input;

	Mat max_img;

	file_select_instruction();
	cout << " or -1 for all average: " << endl;
	cout << " or -2 for save_allDescriptions_YML(): " << endl;
	cout << " or -3 for test from r1c1 to r10c10  : " << endl;
	cout << " or -4 for test all hsv_compare()    : " << endl;

	int index;
	cin >> index;

	if (index == -2){
		save_allDescriptions_YML();
		waitESC();
		return 0;
	}
	else if (index == -1) {
		double cnt = 7;
		double totacc = 0;
		vector<double> accs;
		for (auto idx : valid_indexs) {
			double acc = solve(idx);
			totacc += acc;
			accs.push_back(acc);
		}

		cout << "----------------------------" << endl;
		for (int i = 0; i < accs.size(); ++i) {
			int idx = valid_indexs[i];
			double acc = accs[i];
			cout << "Case " << idx << " acc: " << acc << endl;
		}
		cout << "Total average acc: " << totacc / cnt << endl;
	}
	else if (index == -3){
		printf("pre-load 1000 images for saving time...");
		vector<Mat> imgs = load_imgs(false);

		for (auto v_index : valid_indexs){
			string filepath = getFilePath(v_index);
			Mat src_input = imread(filepath); // read input image

			if (!src_input.data)
			{
				printf("Cannot find the input image!\n");
				continue;
			}


			printf("Work for img \"%s\" :\n", files[v_index].c_str());
			for (int i = 0; i < 11; i++){
				for (int j = 0; j < 10; j++){
					solve(src_input, imgs, v_index, i + 1, j + 1);
				}
				//solve(src_input, imgs, v_index, i + 1, i + 1);
			}
			printf("Done for img \"%s\".\n", files[v_index].c_str());

		}
	}
	else if (index == -4){
		printf("pre-load 1000 images for saving time...");
		vector<Mat> features = load_features();

		for (auto v_index : valid_indexs){
			string filepath = getFilePath(v_index);
			Mat src_input = imread(filepath); // read input image

			if (!src_input.data)
			{
				printf("Cannot find the input image!\n");
				continue;
			}


			printf("Work for img \"%s\" :\n", files[v_index].c_str());
			solve(src_input, features, v_index);
			printf("Done for img \"%s\".\n", files[v_index].c_str());

		}
	}
	else {
		solve(index);
	}


	do{
		cout << "input -1 for exit...\n";
		cin >> index;
	} while (index != -1);

	waitESC();

	return 0;
}


/*
target:

using setting:
img:0 r:5, c:5, #1:T Acc(100): x.xx%, bestP: x.xx%(s:xx.xx), bestR: x.xx%(s:xx.xx)
img:0 r:5, c:6, #1:F Acc(100): x.xx%,
*/