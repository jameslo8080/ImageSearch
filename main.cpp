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
string files[] = {"man", "beach", "building", "bus", "dinosaur", "elephant", "flower", "horse", "mountain", "food"};
int valid_index[] = { 0, 1, 2, 3, 4, 6, 7 };

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
	imshow("Input", src_input);

	// double acc = hsv_compare(src_input, index);
	double acc = hsv_split_compare(src_input, index);
	// double acc = surf_compare(src_input, index);
	// double acc = svm_compare(src_input, index);
	return acc;
}

int main(int argc, char** argv){
	Mat src_input;

	Mat max_img;

	file_select_instruction();
	cout << " or -1 for all average: " << endl;
	cout << " or -2 for save_allDescriptions_YML(): " << endl;

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
		for (auto idx : valid_index) {
			double acc = solve(idx);
			totacc += acc;
			accs.push_back(acc);
		}
		
		cout << "----------------------------" << endl;
		for (int i = 0; i < accs.size(); ++i) {
			int idx = valid_index[i];
			double acc = accs[i];
			cout << "Case " << idx << " acc: " << acc << endl;
		}
		cout << "Total average acc: " << totacc / cnt << endl;
	}
	else {
		solve(index);
	}

	

	waitESC();

	return 0;
}