///**
//* CS4185 Multimedia Technologies and Applications
//* Course Assignment
//* Image Retrieval Project
//*/
//
//#include <iostream>
//#include <stdio.h>
//#include <algorithm>
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/nonfree/nonfree.hpp"
//#include "opencv2/features2d/features2d.hpp"
//
//using namespace std;
//using namespace cv;
//
//#define IMAGE_LIST_FILE "inputimage.txt"  // NOTE: this is relative to current file
//
////Compute pixel-by-pixel difference
//double compareImgs(Mat img1, Mat img2)
//{
//	int w = img1.cols, h = img2.rows;
//	Mat new_img2;
//	resize(img2, new_img2, img1.size());
//	double sum = 0;
//	for (int i = 0; i < w; i++)for (int j = 0; j < h; j++)
//	{
//		sum += abs(img1.at<uchar>(j, i) - new_img2.at<uchar>(j, i));
//	}
//	return sum;
//}
//
//int main(int argc, char** argv)
//{
//	Mat src_input, gray_input;
//	Mat db_img, db_gray_img;
//
//	double score[1000];
//	int db_id = 0;
//	double maxscore = 1000000000;
//	int maxscore_num;
//	char maximg_name[200];
//	Mat max_img;
//
//	src_input = imread("dinosaur.jpg"); // read input image
//	if (!src_input.data)
//	{
//		printf("Cannot find the input image!\n");
//		system("pause");
//		return -1;
//	}
//	imshow("Input", src_input);
//	/// Convert to grayscale
//	cvtColor(src_input, gray_input, COLOR_BGR2GRAY);
//
//	///Read Database
//	FILE* fp;
//	char imagepath[200];
//	fopen_s(&fp, IMAGE_LIST_FILE, "r");
//	printf("Extracting features from input images...\n");
//	while (!feof(fp))
//	{
//		while (fscanf_s(fp, "%s ", imagepath, sizeof(imagepath))   >   0)
//		{
//			printf("%s\n", imagepath);
//			char tempname[200];
//			sprintf_s(tempname, 200, "../%s", imagepath);
//
//			db_img = imread(tempname); // read database image
//			if (!db_img.data)
//			{
//				printf("Cannot find the database image number %d!\n", db_id + 1);
//				system("pause");
//				return -1;
//			}
//
//			cvtColor(db_img, db_gray_img, COLOR_BGR2GRAY);
//
//			/// Apply the pixel-by-pixel comparison method
//			score[db_id] = compareImgs(gray_input, db_gray_img);
//			/// Compute max score
//			if (score[db_id]<maxscore)
//			{
//				maxscore = score[db_id];
//				maxscore_num = db_id;
//				memcpy(maximg_name, tempname, 200 * sizeof(char));
//			}
//			db_id++;
//		}
//	}
//	fclose(fp);
//
//	Mat maximg = imread(maximg_name);
//	imshow("Best Match Image", maximg);
//
//	printf("the most similar image is %d, the pixel-by-pixel difference is %f\n", maxscore_num + 1, maxscore);
//
//	printf("Done \n");
//	// Wait for the user to press a key in the GUI window.
//	//Press ESC to quit
//	int keyValue = 0;
//	while (keyValue >= 0)
//	{
//		keyValue = cvWaitKey(0);
//
//		switch (keyValue)
//		{
//		case 27:keyValue = -1;
//			break;
//		}
//	}
//
//	return 0;
//}
//
//// ********* ********* ********* *********
//// SIFT demo, uncomment below to try it
//// ********* ********* ********* *********
//// int main( int argc, char** argv )
//// {
//
////   Mat img_1 = imread( "./flower.jpg", CV_LOAD_IMAGE_GRAYSCALE );
////   Mat img_2 = imread("../image.orig/685.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//
////   if( !img_1.data || !img_2.data )
////   {
//// 	  printf("Error loading images\n"); return -1;
////   }
//
////   //-- Step 1: Detect the keypoints using SURF Detector
////   int minHessian = 400;
//
////   SurfFeatureDetector detector(minHessian);
//
////   std::vector<KeyPoint> keypoints_1, keypoints_2;
//
////   detector.detect( img_1, keypoints_1 );
////   detector.detect( img_2, keypoints_2 );
//
////   //-- Step 2: Calculate descriptors (feature vectors)
////   SurfDescriptorExtractor extractor;
//
////   Mat descriptors_1, descriptors_2;
//
////   extractor.compute( img_1, keypoints_1, descriptors_1 );
////   extractor.compute( img_2, keypoints_2, descriptors_2 );
//
////   //-- Step 3: Matching descriptor vectors with a brute force matcher
////   BFMatcher matcher(NORM_L2);
////   std::vector< DMatch > matches;
////   matcher.match( descriptors_1, descriptors_2, matches );
//
////   //-- Draw matches
////   Mat img_matches;
////   drawMatches( img_1, keypoints_1, img_2, keypoints_2, matches, img_matches );
//
////   //-- Show detected matches
////   imshow("Matches", img_matches );
//
//// 	printf("Done \n");
//// 	// Wait for the user to press a key in the GUI window.
//// 	//Press ESC to quit
//// 	int keyValue = 0;
//// 	while (keyValue >= 0)
//// 	{
//// 		keyValue = cvWaitKey(0);
//
//// 		switch (keyValue)
//// 		{
//// 		case 27:keyValue = -1;
//// 			break;
//// 		}
//// 	}
//
//// 	return 0;
//// }