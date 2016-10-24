/**
* CS4185 Multimedia Technologies and Applications
* Course Assignment
* Image Retrieval Project
*/

#include <iostream>
#include <string>
#include <stdio.h>
#include <algorithm>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"


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

// ********* ********* ********* *********
// SIFT demo, uncomment below to try it
// ********* ********* ********* *********
int method2(int argc, char** argv){

	Mat img_1 = imread("./flower.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_2 = imread("../image.orig/685.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	if (!img_1.data || !img_2.data)
	{
		printf("Error loading images\n"); return -1;
	}

	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	SurfFeatureDetector detector(minHessian);

	std::vector<KeyPoint> keypoints_1, keypoints_2;

	detector.detect(img_1, keypoints_1);
	detector.detect(img_2, keypoints_2);

	//-- Step 2: Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;

	Mat descriptors_1, descriptors_2;

	extractor.compute(img_1, keypoints_1, descriptors_1);
	extractor.compute(img_2, keypoints_2, descriptors_2);

	//-- Step 3: Matching descriptor vectors with a brute force matcher
	BFMatcher matcher(NORM_L2);
	std::vector< DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	//-- Draw matches
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);

	//-- Show detected matches
	imshow("Matches", img_matches);

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

/**
* @function main
*/
int method3(int argc, char** argv)
{
	Mat src, dst;
	// ask which file
	string file = askFile();

	/// Load image
	src = imread(file, CV_LOAD_IMAGE_COLOR);

	if (!src.data)
	{
		return -1;
	}

	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(src, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}

	/// Display
	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage);

	waitKey(0);

	// hsv
	Mat hsv_base, o_hist;
	cvtColor(src, hsv_base, CV_BGR2HSV);
	vector<Mat> hsv_planes;
	split(hsv_base, hsv_planes);
	/// Compute the histograms:
	int h_bins = 50;
	int s_bins = 32;
	int v_bins = 10;
	int histSize2[] = { h_bins, s_bins, v_bins };

	float h_ranges[] = { 0, 256 };
	float s_ranges[] = { 0, 180 };
	float v_ranges[] = { 0, 256 };

	const float* hhistRange = { h_ranges };
	const float* shistRange = { s_ranges };
	const float* vhistRange = { v_ranges };

	Mat h_hist, s_hist, v_hist;

	calcHist(&hsv_planes[0], 1, 0, Mat(), h_hist, 1, &histSize, &hhistRange, uniform, accumulate);
	calcHist(&hsv_planes[1], 1, 0, Mat(), s_hist, 1, &histSize, &shistRange, uniform, accumulate);
	calcHist(&hsv_planes[2], 1, 0, Mat(), v_hist, 1, &histSize, &vhistRange, uniform, accumulate);

	Mat histImage_hsv(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(h_hist, h_hist, 0, histImage_hsv.rows, NORM_MINMAX, -1, Mat());
	normalize(s_hist, s_hist, 0, histImage_hsv.rows, NORM_MINMAX, -1, Mat());
	normalize(v_hist, v_hist, 0, histImage_hsv.rows, NORM_MINMAX, -1, Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage_hsv, Point(bin_w*(i - 1), hist_h - cvRound(h_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(h_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(histImage_hsv, Point(bin_w*(i - 1), hist_h - cvRound(s_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(s_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(histImage_hsv, Point(bin_w*(i - 1), hist_h - cvRound(v_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(v_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}

	/// Display
	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage_hsv);


	waitKey(0);
	return 0;
}

int method4(int argc, char** argv)
{
	int ib, i1, i2;
	cout << "Input the 3 index of the file that you want to compare (eg: 1 2 3)";
	cin >> ib >> i1 >> i2;

	Mat src_base, hsv_base;
	Mat src_test1, hsv_test1;
	Mat src_test2, hsv_test2;
	Mat hsv_half_down;

	/// Load three images with different environment settings
	// string files[] = { "beach", "building", "bus", "dinosaur", "flower", "horse", "man" };
	src_base = imread(getFilePath(ib), CV_LOAD_IMAGE_COLOR);
	src_test1 = imread(getFilePath999(i1), CV_LOAD_IMAGE_COLOR);
	src_test2 = imread(getFilePath999(i2), CV_LOAD_IMAGE_COLOR);

	/// Convert to HSV
	cvtColor(src_base, hsv_base, COLOR_BGR2HSV);
	cvtColor(src_test1, hsv_test1, COLOR_BGR2HSV);
	cvtColor(src_test2, hsv_test2, COLOR_BGR2HSV);

	hsv_half_down = hsv_base(Range(hsv_base.rows / 2, hsv_base.rows - 1), Range(0, hsv_base.cols - 1));

	/// Using 50 bins for hue and 60 for saturation
	int h_bins = 50; int s_bins = 60;
	int histSize[] = { h_bins, s_bins };

	// hue varies from 0 to 179, saturation from 0 to 255
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges };

	// Use the o-th and 1-st channels
	int channels[] = { 0, 1 };


	/// Histograms
	MatND hist_base;
	MatND hist_half_down;
	MatND hist_test1;
	MatND hist_test2;

	/// Calculate the histograms for the HSV images
	calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, false);
	normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsv_half_down, 1, channels, Mat(), hist_half_down, 2, histSize, ranges, true, false);
	normalize(hist_half_down, hist_half_down, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsv_test1, 1, channels, Mat(), hist_test1, 2, histSize, ranges, true, false);
	normalize(hist_test1, hist_test1, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&hsv_test2, 1, channels, Mat(), hist_test2, 2, histSize, ranges, true, false);
	normalize(hist_test2, hist_test2, 0, 1, NORM_MINMAX, -1, Mat());

	/// Apply the histogram comparison methods
	for (int i = 0; i < 4; i++)
	{
		int compare_method = i;
		double base_base = compareHist(hist_base, hist_base, compare_method);
		double base_half = compareHist(hist_base, hist_half_down, compare_method);
		double base_test1 = compareHist(hist_base, hist_test1, compare_method);
		double base_test2 = compareHist(hist_base, hist_test2, compare_method);

		printf(" Method [%d] Perfect, Base-Half, Base-Test(1), Base-Test(2) : %f, %f, %f, %f \n", i, base_base, base_half, base_test1, base_test2);
	}

	printf("Done \n");

	return 0;
}

int main(int argc, char** argv){

	// ask method
	while (true){

		cout << "Which method to test? (1 ~ 4, -1 to exit): ";
		int method;
		cin >> method;

		switch (method){
		case -1:
			goto exit;
		case 1:
			method1(argc, argv);
			break;
		case 2:
			method2(argc, argv);
			break;
		case 3:
			method3(argc, argv);
			break;
		case 4:
			method4(argc, argv);
			break;
		default:
			break;
		}
	}

exit:
	cout << "Bye~ Bye~";

}

