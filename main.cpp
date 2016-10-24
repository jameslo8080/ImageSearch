/**
* CS4185 Multimedia Technologies and Applications
* Course Assignment
* Image Retrieval Project
*/

#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "feature_extract.h"
#include "feature_compare.h"

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

string files[] = { "beach", "building", "bus", "dinosaur", "flower", "horse", "man" };

string getFilePath(int index){
	if (index < 0 || index >= sizeof(files))
		index = 0;
	string s("./");
	s.append(files[index]).append(".jpg");
	return s;
}

string askFile(){
	cout << "Which file" << "(0:'beach', 1:'building', 2:'bus'," << " 3:'dinosaur', 4:'flower', 5:'horse', 6:'man')? ";
	int index;
	cin >> index;
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

//Compute pixel-by-pixel difference
double compareImgs(Mat img1, Mat img2){
	int w = img1.cols, h = img2.rows;
	Mat new_img2;
	resize(img2, new_img2, img1.size());
	double sum = 0;
	for (int i = 0; i < w; i++)
	for (int j = 0; j < h; j++)	{
		sum += abs(img1.at<uchar>(j, i) - new_img2.at<uchar>(j, i));
	}
	return sum;
}

int main(int argc, char** argv){
	Mat src_input, db_img;

	int db_id = 0;
	Mat max_img;

	cout << "Which file" << "(0:'beach', 1:'building', 2:'bus'," << " 3:'dinosaur', 4:'flower', 5:'horse', 6:'man')? ";
	int index;
	cin >> index;
	string filepath = getFilePath(index);

	src_input = imread(filepath); // read input image
	if (!src_input.data)
	{
		printf("Cannot find the input image!\n");
		system("pause");
		return -1;
	}
	imshow("Input", src_input);

	Mat src_hsv = rgbMat_to_hsvHist(src_input);	
	vector<ImgScore> res0;
	vector<ImgScore> res1;
	vector<ImgScore> res2;
	vector<ImgScore> res3;

	///Read Database
	FILE* fp;
	char imagepath[200];
	fopen_s(&fp, IMAGE_LIST_FILE, "r");
	printf("Extracting features from input images...\n");
	while (!feof(fp))
	{
		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath)) > 0)
		{
			printf("%s\n", imagepath);
			char tempname[200];
			sprintf_s(tempname, 200, "../%s", imagepath);

			db_img = imread(tempname); // read database image
			if (!db_img.data)
			{
				printf("Cannot find the database image number %d!\n", db_id + 1);
				system("pause");
				return -1;
			}

			Mat hist_base = rgbMat_to_hsvHist(db_img);
			res0.push_back(ImgScore(db_id, compareHist(hist_base, src_hsv, 0)));
			res1.push_back(ImgScore(db_id, compareHist(hist_base, src_hsv, 1)));
			res2.push_back(ImgScore(db_id, compareHist(hist_base, src_hsv, 2)));
			res3.push_back(ImgScore(db_id, compareHist(hist_base, src_hsv, 3)));

			db_id++;

		}
	}
	fclose(fp);

	sort(res0.rbegin(), res0.rend());
	sort(res1.rbegin(), res1.rend());
	sort(res2.rbegin(), res2.rend());
	sort(res3.rbegin(), res3.rend());

	res0.resize(10);
	res1.resize(10);
	res2.resize(10);
	res3.resize(10);
	printf("res0¡@Acc: %lf \n", validate_fit(res0, index));
	printf("res1¡@Acc: %lf \n", validate_fit(res1, index));
	printf("res2¡@Acc: %lf \n", validate_fit(res2, index));
	printf("res3¡@Acc: %lf \n", validate_fit(res3, index));
	printf("Done \n");

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

	return 0;
}
