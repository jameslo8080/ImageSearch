#include "feature_extract.h"
#include "compare.h"
#include "ScoreReport.h"

double double_compare_flower(Mat src_input, vector<vector<Mat>> features, int inputIndex) {


	vector<Mat> imgs_f = divide_image(src_input, 4, 4);
	vector<Mat> imgs_o = imgs_f;
	vector<Mat> SURFDescriptors;
	// only need 6tt,7th,10th,11th
	// erase the first 5 elements:
	imgs_f.erase(imgs_f.begin(), imgs_f.begin() + 5);
	imgs_f.erase(imgs_f.begin() + 2, imgs_f.begin() + 4);
	imgs_f.resize(4);

	imgs_o.erase(imgs_o.begin() + 9, imgs_o.begin() + 9 + 2);
	imgs_o.erase(imgs_o.begin() + 5, imgs_o.begin() + 5 + 2);

	for each (Mat i_f in imgs_f)
	{
		SURFDescriptors.push_back(calSURFDescriptor(i_f));
	}


	//// compare hsv
	Mat src_hsv = rgbMat_to_hsvHist(src_input);
	vector<ImgScore> imgScoreResult;
	for (int i = 0; i < features.size(); ++i)
		imgScoreResult.push_back(ImgScore(i, compareHist(features[i], src_hsv, 0)));

	// sort
	sort(imgScoreResult.rbegin(), imgScoreResult.rend());

	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return 0;
}

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
double double_compare_bus() {
	//Mat src_input, vector<vector<Mat>> features, int inputIndex
	//	Mat bus_1 = imread("./precut/bus_1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat bus_1 = imread("./precut/bus_3.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//bus_1 = imread("./bus.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	// imread("./precut/bus_1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	// Mat bus_2 = imread("./precut/bus_2.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	/*vector<Mat> busPreFeature = {
		imread("./precut/bus_1.jpg", CV_LOAD_IMAGE_GRAYSCALE)
		, imread("./precut/bus_2.jpg", CV_LOAD_IMAGE_GRAYSCALE)
		};*/
	bool showImg = false;
	int start = 0, end = 1000;
	int minHessian;

	double bestAcc = 0, bestMinH = 0;
	int useCase = 0;

	for (int j = 0; j < 3; j++){
		minHessian = 28 + 4 * j;
		SurfFeatureDetector detector(minHessian);
		std::vector<KeyPoint> keypoints_1, keypoints_2;
		detector.detect(bus_1, keypoints_1);

		Mat descriptors_1, descriptors_dbimg;
		SurfDescriptorExtractor extractor;
		extractor.compute(bus_1, keypoints_1, descriptors_1);

		vector<ImgScore> iss;
		vector<int> eIndex;
		for (int i = start; i < end; i++){
			try{
				string file = "../image.orig/" + to_string(i) + ".jpg";
				Mat db_img = imread(file.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

				if (!bus_1.data || !db_img.data)
				{
					printf("Error loading images\n"); return -1;
				}

				//vector<Mat> SURFDescriptors;
				//SURFDescriptors.push_back(calSURFDescriptor(bus_1));
				//SURFDescriptors.push_back(calSURFDescriptor(bus_2));

				//-- Step 1: Detect the keypoints using SURF Detector
				detector.detect(db_img, keypoints_2);

				//-- Step 2: Calculate descriptors (feature vectors)
				extractor.compute(db_img, keypoints_2, descriptors_dbimg);

				//-- Step 3: Matching descriptor vectors with a brute force matcher
				BFMatcher matcher(NORM_L2);
				// BFMatcher matcher(NORM_HAMMING);
				// FlannBasedMatcher matcher;

				std::vector<DMatch> matches;
				matcher.match(descriptors_1, descriptors_dbimg, matches);

				//-- Draw matches

				if (showImg){
					Mat img_matches;
					drawMatches(bus_1, keypoints_1, db_img, keypoints_2, matches, img_matches);
					imshow("Matches", img_matches);

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


				double score = descriptors_cal_match(descriptors_1, descriptors_dbimg);
				iss.push_back(ImgScore(i, score));
				//printf(" img#%i, match score:%f \n", i, score);
				if (i % 10 == 0)
					printf(" %i%%..", i / 10);
				//-- Show detected matches
				// imshow("Matches", img_matches);
				// imshow("bus_1", bus_1);
			}
			catch (Exception e){
				printf(" -----ERROR-----img#%i, %s\n", i, e.msg);
				eIndex.push_back(i);
			}
		}
		printf("\nsorting...\n");
		sort(iss.begin(), iss.end());

		printf("Sorted result (bus: 300 ~ 399):");
		int times = (iss.size() > 180) ? 180 : iss.size();
		for (int i = 0; i < times; i++){
			if (i % 3 == 0)
				cout << endl;
			printf(" index:%3i, img#%3i, MScore:%f |", i, iss[i].db_id, iss[i].score);
			(iss[i].db_id / 100 == 3) ? cout << " T |" : cout << "   |";
		}
		printf("-----\n");

		if (eIndex.size() > 0){
			printf("error: ");
			for (int i : eIndex)
				printf("%i .jpg, \n", i);
		}

		ScoreReport sr(iss, 3);
		sr.report();
		printf("-----\n");

		if (sr.acc100 > bestAcc){
			bestAcc = sr.acc100;
			bestMinH = j;
		}
	}// for j
	printf("best minH j=%i Acc:%f%% \n", bestMinH, bestAcc);

	printf("-1 to exit");
	int input;
	do{
		cin >> input;
	} while (input != -1);
	//printf("Done \n");

	return 0;
}


int bababa(int argc, char** argv)
{

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

//
//double hsv_split_compare(Mat src_input, vector<Mat> all_img, int inputIndex, bool output, int row, int col) {
//	vector<Mat> imgs = all_img;
//
//	int tot = row*col;
//	vector<Mat> src_hsv = rgbMat_to_divided_hsvHist(src_input, row, col);
//
//	vector<ImgScore> imgScoreResult;
//
//	for (int i = 0; i < imgs.size(); ++i) {
//		vector<Mat> splited_img = rgbMat_to_divided_hsvHist(imgs[i], row, col);
//		double scores = 0;
//		double sums = 0;
//		for (int i = 0; i < tot; ++i)
//			sums += compareHist(splited_img[i], src_hsv[i], 0);
//		imgScoreResult.push_back(ImgScore(i, sums / (double)(tot)));
//	}
//
//	// sort
//	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
//
//	ScoreReport sr(imgScoreResult, inputIndex);
//	sr.report(row, col);
//
//	return 0;
//}