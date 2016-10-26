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
<<<<<<< HEAD
#include "opencv2/features2d/features2d.hpp" 
#include "ImageScore.h"
=======
#include "opencv2/features2d/features2d.hpp"
#include "feature_loader.h"
#include "feature_extract.h"
#include "feature_compare.h"
#include "ml.h"
>>>>>>> origin/master

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


bool isMatch(const ImageScore& imgsc, int targetClass){
	//return targetClass == imgsc.classID();

	switch (targetClass){
	case 0:
		if (imgsc.id >= 100 && imgsc.id <= 199)
			return true;
		break;
	case 1:
		if (imgsc.id >= 200 && imgsc.id <= 299)
			return true;
		break;
	case 2:
		if (imgsc.id >= 300 && imgsc.id <= 399)
			return true;
		break;
	case 3:
		if (imgsc.id >= 400 && imgsc.id <= 499)
			return true;
		break;
	case 4:
		if (imgsc.id >= 600 && imgsc.id <= 699)
			return true;
		break;
	case 5:
		if (imgsc.id >= 700 && imgsc.id <= 799)
			return true;
		break;
	case 6:
		if (imgsc.id >= 0 && imgsc.id <= 100)
			return true;
		break;
	}
	return false;

}


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

<<<<<<< HEAD
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


int method1(int argc, char** argv){
	Mat src_input, gray_input;
	Mat db_img, db_gray_img;

	double score[1000];
	int db_id = 0;
	double maxscore = 1000000000;
	int maxscore_num;
	char maximg_name[200];
	Mat max_img;

	src_input = imread("dinosaur.jpg"); // read input image
	if (!src_input.data)
	{
		printf("Cannot find the input image!\n");
		system("pause");
		return -1;
	}
	imshow("Input", src_input);
	/// Convert to grayscale
	cvtColor(src_input, gray_input, COLOR_BGR2GRAY);

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

			cvtColor(db_img, db_gray_img, COLOR_BGR2GRAY);

			/// Apply the pixel-by-pixel comparison method
			score[db_id] = compareImgs(gray_input, db_gray_img);
			/// Compute max score

			if (score[db_id] < maxscore)
			{
				maxscore = score[db_id];
				maxscore_num = db_id;
				memcpy(maximg_name, tempname, 200 * sizeof(char));
			}
			db_id++;
		}
	}
	fclose(fp);

	Mat maximg = imread(maximg_name);
	imshow("Best Match Image", maximg);

	printf("the most similar image is %d, the pixel-by-pixel difference is %f\n", maxscore_num + 1, maxscore);

	printf("Done \n");
=======
void waitESC() {
>>>>>>> origin/master
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
	printf("res0¡@Acc: %lf \n", validate_fit(res0, index));
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
	printf("res0¡@Acc: %lf \n", validate_fit(scores, index));
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
