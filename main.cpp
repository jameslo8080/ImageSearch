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

void solve(int index) {
	Mat src_input = get_input_img(index);
	vector<int> res = new_compare(src_input, index, 6.619589);
	save_result(res);
}

void extra_fucntion() {

}


int main(int argc, char** argv) {
	Mat src_input;

	Mat max_img;

	file_select_instruction();
	int instruction;
	cin >> instruction;
	solve(instruction);

	cout << "Done!\n";
	waitESC();
	return 0;
}
