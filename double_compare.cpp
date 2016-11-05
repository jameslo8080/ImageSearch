#include "feature_extract.h"
#include "compare.h"
#include "ScoreReport.h"
#include <ctime>

Mat test(Mat src, int hbins, int sbins) {
	Mat hsv;

	cvtColor(src, hsv, CV_BGR2HSV);

	// Quantize the hue to 30 levels
	// and the saturation to 32 levels
	//int hbins = 30, sbins = 32;

	int histSize[] = { hbins, sbins };
	// hue varies from 0 to 179, see cvtColor
	float hranges[] = { 0, 180 };
	// saturation varies from 0 (black-gray-white) to
	// 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	MatND hist;
	// we compute the histogram from the 0-th and 1-st channels
	int channels[] = { 0, 1 };

	calcHist(&hsv, 1, channels, Mat(), // do not use mask
			 hist, 2, histSize, ranges,
			 true, // the histogram is uniform
			 false);
	if (!true) {
		double maxVal = 0;
		minMaxLoc(hist, 0, &maxVal, 0, 0);

		int scale = 10;
		Mat histImg = Mat::zeros(sbins*scale, hbins * 10, CV_8UC3);

		for (int h = 0; h < hbins; h++)
			for (int s = 0; s < sbins; s++) {
				float binVal = hist.at<float>(h, s);
				int intensity = cvRound(binVal * 255 / maxVal);
				rectangle(histImg, Point(h*scale, s*scale),
						  Point((h + 1)*scale - 1, (s + 1)*scale - 1),
						  Scalar::all(intensity),
						  CV_FILLED);
			}

		namedWindow("Source", 1);
		imshow("Source", src);

		namedWindow("H-S Histogram", 1);
		imshow("H-S Histogram", histImg);
		waitKey();
	}
	return hist;
}
void double_compare() {
	using namespace std;
	clock_t begin = clock();


	on_double_compare();

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	printf("\nused time:%f\n", elapsed_secs);
}
void reportImgError(vector<int> eIndex) {
	if (eIndex.size() > 0) {
		printf("=====-----ERROR-----=====: ");
		for (int i : eIndex)
			printf("%3i.jpg, \n", i);
	}
}

Mat calSURFDescriptor_one(Mat input, int minH = 8) {
	SurfFeatureDetector detector(minH);
	SurfDescriptorExtractor extractor;
	vector<KeyPoint> keyPoints;
	Mat descriptors_result;
	detector.detect(input, keyPoints);
	extractor.compute(input, keyPoints, descriptors_result);
	return descriptors_result;
}

vector<Mat> calSURFDescriptor_vector(vector<Mat> inputs, int minH = 8) {
	SurfFeatureDetector detector(minH);
	SurfDescriptorExtractor extractor;
	vector<KeyPoint> keyPoints;
	vector<Mat> descriptors_results(inputs.size());
	for (int i = 0; i < inputs.size(); i++) {
		detector.detect(inputs[i], keyPoints);
		extractor.compute(inputs[i], keyPoints, descriptors_results[i]);
	}
	return descriptors_results;
}
struct bestMethodData {
	double bestGradeAP = 0, bestGradeAR = 0, score = 0;
	int method = 0; // sort method
	int bins[2];
	int minH = 0;

	void update(ScoreReport sr, int _method, int hbins, int sbins, int minHe) {
		sr.reportSorted(100);
		sr.report();
		bestMethodData bmd;
		if (sr.bestGradeAindex != -1 && sr.bestGradeAPR() > (bestGradeAP * bestGradeAR)) {
			bestGradeAP = sr.bestGradeAP;
			bestGradeAR = sr.bestGradeAR;
			score = sr.score(sr.bestGradeAindex);
			method = _method;
			bins[0] = hbins;
			bins[1] = sbins;
			minH = minHe;
		}
		if (method <= 0 && sr.acc100 > score) {
			score = sr.acc100;
			method = _method * -1;
			bins[0] = hbins;
			bins[1] = sbins;
			minH = minHe;
		}
	}
	void report() {
		printf(" - Current best hbins: %i , sbins: %i , method: %i , minH: %i \n", bins[0], bins[1], method, minH);
		printf(" - Current best bestGradeAP: %f , GradeAR: %f , score: %f \n\n", bestGradeAP, bestGradeAR, score);
	}
};

Mat cutMiddle(Mat src) {
	int x = src.cols / 5, y = src.rows / 5, w = src.cols - (2 * x), h = src.rows - (2 * y);
	return Mat(src, Rect(x, y, w, h));
	/*imshow("result", result);
	cvWaitKey();
	return result;*/
}

void on_double_compare() {
declare:
	int inputIndex = 3;
	int hbins_[3] = { 8, 32, 4 };
	int sbins_[3] = { 8, 32, 4 };
	int minH_[3] = { 240, 300, 20 };

	bool useHsv = true, useFea = false; // not ready

start:
	cout << endl << "double_compare:::::" << endl;
	string files[] = { "man", "beach", "building", "bus", "dinosaur", "elephant", "flower", "horse", "mountain", "food" };

	string src = "./" + files[inputIndex] + ".jpg";

	Mat src_color = imread(src.c_str(), CV_LOAD_IMAGE_COLOR);
	Mat src_gray = imread(src.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

	int x1 = src_color.cols / 5, y1 = src_color.rows / 5, x2 = src_color.cols - (x1), y2 = src_color.rows - (y1);

	//cv::Rect const mask(x1, y1, x2, y2);
	//cv::Mat roi = src_color(mask);
	//imshow("roi", roi);
	//cvWaitKey();

	bestMethodData bestMData;
	for (int minH = minH_[0]; minH <= minH_[1]; minH += minH_[2]) {
		for (int hbins = hbins_[0]; hbins <= hbins_[1]; hbins += hbins_[2]) {
			for (int sbins = sbins_[0]; sbins <= sbins_[1]; sbins += sbins_[2]) {


				Mat src_hsv = test(cutMiddle(src_color), hbins, sbins);

				//Mat(raw_img, Rect(i*iw, j*ih, iw, ih))); // add .clone() wull be deep copy
				//vector<Mat> imgs_fea = divide_image(src_input, 4, 4);

				Mat src_used = src_color;
				Mat src_fea = cutMiddle(src_used);

				//imshow("src_fea", src_fea);
				//cvWaitKey();

				//flower_hsv = test(src_fea);
				//imshow("cut_input", src_fea);
				//cvWaitKey();

				// only need 6tt,7th,10th,11th
				// erase the first 5 elements:
				Mat input_descriptor;
				if (useFea) {
					input_descriptor = calSURFDescriptor_one(src_fea, minH);
				}
				vector<int> errorIndex;
				vector<ImgScore> iss;
				for (int fileIndex = 0; fileIndex < 1000; fileIndex++) {
					try {
						// hsv
						string file = "../image.orig/" + to_string(fileIndex) + ".jpg";

						Mat dbimg_src_color = cutMiddle(imread(file.c_str(), CV_LOAD_IMAGE_COLOR));
						//Mat dbimg_src_gray = imread(file.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
						Mat dbimg_hsv = test(dbimg_src_color, hbins, sbins);
						//rgbMat_to_hsvHist(imread(file.c_str(), 1));

						double hsv_score = 0;
						if (useHsv) { hsv_score = compareHist(src_hsv, dbimg_hsv, CV_COMP_CORREL); }
						//cout << "hsv_score:" << hsv_score << endl;
						//fea
						//vector<Mat> dbimg_fea = divide_image(dbimg_src, 4, 4);
						//dbimg_fea.erase(dbimg_fea.begin(), dbimg_fea.begin() + 5);
						//dbimg_fea.erase(dbimg_fea.begin() + 2, dbimg_fea.begin() + 4);
						//dbimg_fea.resize(4);

						// ===== Feature =====
						double fea_score = 0;
						Mat dmimg_descriptor;
						if (useFea) {
							dmimg_descriptor = calSURFDescriptor_one(dbimg_src_color, minH);
							fea_score = descriptors_cal_match(input_descriptor, dmimg_descriptor);
						}
						//for (int i = 0; i < input_descriptors.size(); i++) {
						// fea_score += descriptors_cal_match(input_descriptors[i], dmimg_descriptor);
						//}
						//fea_score /= (double) input_descriptors.size() * (input_descriptors.size() + 1) / 2;
						//fea_score /= input_descriptors.size();
						//cout << "fea_score:" << fea_score << endl;

						double calScore = 0;

						if (useHsv && useFea) {
							calScore = hsv_score + ((1 - fea_score) * 2);
						} else if (useHsv) {
							calScore = hsv_score;
						} else if (useFea) {
							calScore = fea_score;
						}

						iss.push_back(ImgScore(fileIndex, calScore));

					} catch (Exception e) {
						printf(" -----ERROR-----img#%i, %s\n", fileIndex, e.msg);
						errorIndex.push_back(fileIndex);
					}

					if (fileIndex % 10 == 0)
						printf("%i%% ", (fileIndex + 1) / 10);
				}
				cout << endl;

				//// compare hsv
				//Mat src_hsv = rgbMat_to_hsvHist(src_input);

				//for (int i = 0; i < features.size(); ++i)
				//	imgScoreResult.push_back(ImgScore(i, compareHist(features[i], src_hsv, 0)));

				reportImgError(errorIndex);

				ScoreReport sr;
				// ===== sort method 1 // for fea?
				sort(iss.begin(), iss.end());
				sr = ScoreReport(iss, inputIndex);
				bestMData.update(sr, 1, hbins, sbins, minH);

				// ===== sort method 2 // for hsv?
				sort(iss.rbegin(), iss.rend());
				sr = ScoreReport(iss, inputIndex);
				bestMData.update(sr, 2, hbins, sbins, minH);

				// ===== sort method 3
				for (ImgScore &is : iss) {
					is.score = abs(is.score);
				}
				sort(iss.begin(), iss.end());
				sr = ScoreReport(iss, inputIndex);
				bestMData.update(sr, 3, hbins, sbins, minH);

				// ===== sort method 4
				sort(iss.rbegin(), iss.rend());
				sr = ScoreReport(iss, inputIndex);
				bestMData.update(sr, 4, hbins, sbins, minH);

				printf(" - ===== End of minHessian=%i =====\n", minH);
				printf(" - ===== Finished hbin : %i , sbin: %i \n", hbins, sbins);
				bestMData.report();
				if (!useHsv) { goto fea_only; }
			} // sbins
		} // hbins
	fea_only:
		if (!useFea) { break; }
	} // minH
}

/*
man			: 0 ~ 99
beach		: 101 ~ 199
building	: 200 ~ 299
bus			: 300 ~ 399
dinosaur	: 400 ~ 499
elephant	: 500 ~ 599
flower		: 600 ~ 699
horse		: 700 ~ 799
mountain	: 800 ~ 899
food		: 900 ~ 999
*/
void double_compare_bus() {
	// Mat src_input, vector<vector<Mat>> features, int inputIndex
	// Mat bus_1 = imread("./precut/bus_1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	// Mat bus_1 = imread("./precut/bus_3.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	// Mat bus_1 = imread("./bus.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	// imread("./precut/bus_1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	// Mat bus_2 = imread("./precut/bus_2.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	vector<Mat> busPreFeature = {
		imread("./precut/bus_1.jpg", CV_LOAD_IMAGE_GRAYSCALE)
		, imread("./precut/bus_3.jpg", CV_LOAD_IMAGE_GRAYSCALE)
	};


	bool showImg = false;
	int start = 0, end = 1000;
	int minHessian;

	double bestAcc = 0;
	int bestMinH = 0;
	int useCase = 0;

	for (int j = 0; j < 100; j++) {
		minHessian = 4 + 4 * j;

		SurfFeatureDetector detector(minHessian);
		SurfDescriptorExtractor extractor;

		vector<std::vector<KeyPoint>> precut_keypoints(busPreFeature.size());
		vector<Mat> precut_descriptors(busPreFeature.size());

		for (int i = 0; i < busPreFeature.size(); i++) {
			detector.detect(busPreFeature[i], precut_keypoints[i]);
			extractor.compute(busPreFeature[i], precut_keypoints[i], precut_descriptors[i]);
		}


		//std::vector<KeyPoint> keypoints_1, keypoints_2;
		//detector.detect(bus_1, keypoints_1);

		std::vector<KeyPoint>keypoints_2;

		Mat descriptors_dbimg;



		vector<ImgScore> iss;
		vector<int> eIndex;
		for (int i = start; i < end; i++) {
			try {
				string file = "../image.orig/" + to_string(i) + ".jpg";
				Mat db_img = imread(file.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

				if (!db_img.data) {
					printf("Error loading images\n"); return;
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
				for (int i = 0; i < busPreFeature.size(); i++) {
					//matcher.match(descriptors_1, descriptors_dbimg, matches);
					matcher.match(precut_descriptors[i], descriptors_dbimg, matches);
				}


				//-- Draw matches

				if (showImg) {
					Mat img_matches;
					for (int i = 0; i < busPreFeature.size(); i++) {
						drawMatches(busPreFeature[i], precut_keypoints[i], db_img, keypoints_2, matches, img_matches);

						//drawMatches(bus_1, keypoints_1, db_img, keypoints_2, matches, img_matches);
						imshow("Matches", img_matches);

						int keyValue = 0;
						while (keyValue >= 0) {
							keyValue = cvWaitKey(0);

							switch (keyValue) {
								case 27:keyValue = -1;
									break;
							}
						}
					}
				}


				//double score = descriptors_cal_match(descriptors_1, descriptors_dbimg);
				double score = 0;
				for (int i = 0; i < busPreFeature.size(); i++) {
					score += descriptors_cal_match(precut_descriptors[i], descriptors_dbimg);
				}
				score /= busPreFeature.size();
				iss.push_back(ImgScore(i, score));
				//printf(" img#%i, match score:%f \n", i, score);
				if (i % 10 == 0)
					printf(" %i%%..", i / 10);
				//-- Show detected matches
				// imshow("Matches", img_matches);
				// imshow("bus_1", bus_1);
			} catch (Exception e) {
				printf(" -----ERROR-----img#%i, %s\n", i, e.msg);
				eIndex.push_back(i);
			}
		}
		printf("\n\n");

		// Sort
		sort(iss.begin(), iss.end());

		reportImgError(eIndex);
		printf("-----\n");

		ScoreReport sr(iss, 3);
		sr.reportSorted(300);
		sr.report();


		printf("=====End of minHessian=%3i\n=====\n", minHessian);
		//cout << "acc:" << sr.acc100 <<" , bestAcc" << bestAcc;
		if (sr.acc100 > bestAcc) {
			bestAcc = sr.acc100;
			bestMinH = minHessian;
		}
		printf("-Current best minH:%i, Acc100:%.0f", bestMinH, bestAcc);
	}// for j
	printf("best minH=%i Acc: %.4f%% | ", bestMinH, bestAcc);

	printf("-1 to exit");
	int input;
	do {
		cin >> input;
	} while (input != -1);


}


//
//int bababa(int argc, char** argv) {
//
//	Mat img_1 = imread("./flower.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	Mat img_2 = imread("../image.orig/685.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//
//	if (!img_1.data || !img_2.data) {
//		printf("Error loading images\n"); return -1;
//	}
//
//	//-- Step 1: Detect the keypoints using SURF Detector
//	int minHessian = 400;
//
//	SurfFeatureDetector detector(minHessian);
//
//	std::vector<KeyPoint> keypoints_1, keypoints_2;
//
//	detector.detect(img_1, keypoints_1);
//	detector.detect(img_2, keypoints_2);
//
//	//-- Step 2: Calculate descriptors (feature vectors)
//	SurfDescriptorExtractor extractor;
//
//	Mat descriptors_1, descriptors_2;
//
//	extractor.compute(img_1, keypoints_1, descriptors_1);
//	extractor.compute(img_2, keypoints_2, descriptors_2);
//
//	//-- Step 3: Matching descriptor vectors with a brute force matcher
//	BFMatcher matcher(NORM_L2);
//	std::vector< DMatch > matches;
//	matcher.match(descriptors_1, descriptors_2, matches);
//
//	//-- Draw matches
//	Mat img_matches;
//	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
//
//	//-- Show detected matches
//	imshow("Matches", img_matches);
//
//	printf("Done \n");
//	// Wait for the user to press a key in the GUI window.
//	//Press ESC to quit
//	int keyValue = 0;
//	while (keyValue >= 0) {
//		keyValue = cvWaitKey(0);
//
//		switch (keyValue) {
//			case 27:keyValue = -1;
//				break;
//		}
//	}
//
//	return 0;
//}

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