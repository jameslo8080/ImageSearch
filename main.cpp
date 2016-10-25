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

void hsv_compare(Mat src_input, int index) {

	//// compare hsv
	Mat src_hsv = rgbMat_to_hsvHist(src_input);	
	vector<ImgScore> res0;
	vector<Mat> features = load_features();
	for (int i = 0; i < features.size(); ++i) res0.push_back(ImgScore(i, compareHist(features[i], src_hsv, 0)));
	sort(res0.rbegin(), res0.rend());
	res0.resize(10);
	printf("res0�@Acc: %lf \n", validate_fit(res0, index));
	printf("Done \n");
}

int main(int argc, char** argv){
	Mat src_input;

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

	// -- hsv compare approach --
	// hsv_compare(src_input, index);

	// *********** Do compare surf here ***************
	// need to call save_allDescriptions_YML() once, before run this
	vector<Mat> allDescriptors = load_allDescriptions_YML();  // <------------------ here have BUG. cannot really log the MAT

	Mat inputDescriptor = calSURFDescriptor(src_input);
	vector<ImgScore> scores;
	for (int i = 0; i < allDescriptors.size(); ++i) {
		ImgScore sc;
		sc.db_id = i;
		sc.score = surf_compare(inputDescriptor, allDescriptors[i]);

		scores.push_back(sc);
	}
	sort(scores.rbegin(), scores.rend());

	scores.resize(10);
	printf("res0�@Acc: %lf \n", validate_fit(scores, index));
	printf("Done \n");
	// ************************************************

	waitESC();

	return 0;
}


/**          The code to run clustering  - SURF + SVM approach
             Too slow in training, so give up

cout << "Train BOW" << endl;
Mat dictionary = trainBOW(allDescriptors);
Ptr<DescriptorExtractor> extractor = DescriptorMatcher::create("SIFT");
Ptr<DescriptorMatcher>  matcher = DescriptorExtractor::create("BruteForce");
BOWImgDescriptorExtractor bowExtractor(extractor, matcher);
bowExtractor.setVocabulary(dictionary);
cout << "load Samples" << endl;
map<int, Mat> samples = load_mlSample(bowExtractor);
map<int, Ptr<SVM>> svms;
cout << "train Svm" << endl;
for (int i = 0; i < 10; ++i) {
CvSVMParams svmParams;
svms[i] = new SVM;
trainSvm(samples, i, svmParams, svms[i]);
string fname = to_string(i);
string extension(".xml");
string fullname = fname + extension;
svms[i]->save(fullname.c_str());
}
cout << "Predict" << endl;
Mat src_descriptor = cal_descriptor(bowExtractor, src_input);
int result = classifyBySvm(svms, src_descriptor);
cout << "svm result " << result << endl;
*/
