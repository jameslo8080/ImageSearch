#include "feature_extract.h"
#include "compare.h"
#include "ScoreReport.h"
#include <ctime>

Mat img_src_to_hsv(Mat src, int hbins, int sbins) {
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

Mat calSURFDescriptor_one(Mat input, int minH) {
	SurfFeatureDetector detector(minH);
	SurfDescriptorExtractor extractor;
	vector<KeyPoint> keyPoints;
	Mat descriptors_result;
	detector.detect(input, keyPoints);
	extractor.compute(input, keyPoints, descriptors_result);
	return descriptors_result;
}

vector<Mat> calSURFDescriptor_vector(vector<Mat> inputs, int minH) {
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
	int method = 0; // method
	int bins[2];
	int minH = 0;
	bool a_method[16];
	int firstWrong;
	ScoreReport save_sr;

	bestMethodData() { for (int i = 0; i < sizeof(a_method); i++) { a_method[i] = false; } }

	void update(ScoreReport sr, int _method, int hbins, int sbins, int minHe) {
		sr.reportSorted(100);
		sr.report();
		bestMethodData bmd;
		if (sr.bestGradeAindex != -1) {
			a_method[_method - 1] = true;
		}

		if (sr.bestGradeAindex != -1 && sr.bestGradeAPR() >(bestGradeAP * bestGradeAR)) {
			bestGradeAP = sr.bestGradeAP;
			bestGradeAR = sr.bestGradeAR;
			score = sr.score(sr.bestGradeAindex);
			save_sr = sr;
			method = _method;
			bins[0] = hbins;
			bins[1] = sbins;
			minH = minHe;
		}
		if (method <= 0 && sr.acc100 >= score) {
			score = sr.acc100;
			save_sr = sr;
			method = _method * -1;
			bins[0] = hbins;
			bins[1] = sbins;
			minH = minHe;
		}
	}
	void report() {
		cout << " - A method (1~n): ";
		for (int i = 0; i < 16; i++) { if (a_method[i])	cout << (i + 1) << ", "; }
		cout << endl;
		printf(" - Current best hbins: %i , sbins: %i , method: %i , minH: %i \n", bins[0], bins[1], method, minH);
		printf(" - Current best bestGradeAP: %.4f%% , GradeAR: %.4f%% , score: %.4f , firstWrong:%i  \n\n", bestGradeAP, bestGradeAR, score, save_sr.firstWrong);
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
	int debugIndex = -1;
	//int inputIndex = -1;
	int hbins_[3] = { 24, 24, 8 };
	int sbins_[3] = { 28, 28, 12 };
	int minH_[3] = { 64, 64, 64 };
	int hsv_divide_[2] = { 4, 4 };
	int fea_divide_[2] = { 9, 9 };
	bool useHsv = true, useFea = true; // not ready 

	vector<bestMethodData> bestMData_(8);
	string files[] = { "man", "beach", "building", "bus", "dinosaur", "elephant", "flower", "horse", "mountain", "food" };
	int valid_indexs[] = { 0, 1, 2, 3, 4, 6, 7 }; // 0, 1, 2, 3, 4, 6, 7

eachInput:
	//for each (int valid_index in valid_indexs) {
	//	inputIndex = valid_index;
	// for DEBUG
	//if (debugIndex != 1 && debugIndex != inputIndex)
	// continue;

start:
	cout << endl << "double_compare:::::" << endl;
	vector<Mat> src_color_(8);
	vector<Mat> src_gray_(8);
	vector<Mat> src_color_middle_(8);
	vector<Mat> src_gray_middle_(8);

	for each (int valid_index in valid_indexs) {
		string src = "./" + files[valid_index] + ".jpg";
		Mat src_color = imread(src.c_str(), CV_LOAD_IMAGE_COLOR);
		Mat src_gray = imread(src.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		Mat src_color_middle = cutMiddle(src_color);
		Mat src_gray_middle = cutMiddle(src_gray);

		src_color_[valid_index] = src_color;
		src_gray_[valid_index] = src_gray;
		src_color_middle_[valid_index] = src_color_middle;
		src_gray_middle_[valid_index] = src_gray_middle;
	}

	//int x1 = src_color.cols / 5, y1 = src_color.rows / 5, x2 = src_color.cols - (x1), y2 = src_color.rows - (y1);

	//bestMethodData bestMData;
	for (int minH = minH_[0]; minH <= minH_[1]; minH += minH_[2]) {
		for (int hbins = hbins_[0]; hbins <= hbins_[1]; hbins += hbins_[2]) {
			for (int sbins = sbins_[0]; sbins <= sbins_[1]; sbins += sbins_[2]) {

				// ===== Divide =====
				vector<vector<Mat>> src_color_divide_(8);
				// for SURF feature
				vector<vector<Mat>> src_gray_divide_(8);

				// ===== HSV =====
				vector<Mat> src_hsv_(8);
				vector<Mat> src_middle_hsv_(8);
				vector<vector<Mat>> src_color_divide_hsv_(8);


				for each (int i in valid_indexs) {
					src_color_divide_[i] = divide_image(src_color_[i], hsv_divide_[0], hsv_divide_[1]);
					src_gray_divide_[i] = divide_image(src_gray_[i], fea_divide_[0], fea_divide_[1]);

					src_hsv_[i] = img_src_to_hsv(src_color_[i], hbins, sbins);
					src_middle_hsv_[i] = img_src_to_hsv(src_color_middle_[i], hbins, sbins);

					for (int j = 0; j < src_color_divide_[i].size(); j++) {
						src_color_divide_hsv_[i].push_back(img_src_to_hsv(src_color_divide_[i][j], hbins, sbins));
					}

				}

				// ===== Feature descriptor =====
				vector<Mat> src_g_m_descriptor_(8);
				vector<vector<Mat>> src_d_descriptors_(8);
				if (useFea) {
					for each (int i in valid_indexs) {
						src_g_m_descriptor_[i] = calSURFDescriptor_one(src_gray_middle_[i], minH);

						for (int j = 0; j < src_gray_divide_[i].size(); j++) {
							Mat temp_desc = calSURFDescriptor_one(src_gray_divide_[i][j], minH);
							// it seems some img may not able to find any descriptor when it is too small
							// we just ignore this currently
							if (!temp_desc.empty())
								src_d_descriptors_[i].push_back(temp_desc);
							else {
								cout << "log:empty:" << i << ":" << j << endl;
							}
						}
					}

				}
				vector<int> errorIndex;

				vector<vector<vector<ImgScore>>> iss_list_(8);
				for (int i = 0; i < iss_list_.size(); i++) {
					iss_list_[i] = vector<vector<ImgScore>>(15);
				}

				for (int fileIndex = 0; fileIndex < 1000; fileIndex++) {
					try {
						// ===== Load file =====
						string file = "../image.orig/" + to_string(fileIndex) + ".jpg";

						Mat dbimg_color = imread(file.c_str(), CV_LOAD_IMAGE_COLOR);
						Mat dbimg_gray = imread(file.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
						Mat dbimg_src_color_middle = cutMiddle(imread(file.c_str(), CV_LOAD_IMAGE_COLOR));
						//Mat dbimg_src_gray = imread(file.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

						// ===== Divide (for HSV) =====
						vector<Mat> dbimg_color_divide = divide_image(dbimg_color, hsv_divide_[0], hsv_divide_[1]);
						vector<Mat> dbimg_color_divide_hsv;
						Mat dbimg_hsv_middle;
						if (useHsv)
							for (int i = 0; i < dbimg_color_divide.size(); i++) {
								dbimg_color_divide_hsv.push_back(img_src_to_hsv(dbimg_color_divide[i], hbins, sbins));

								// middle
								dbimg_hsv_middle = img_src_to_hsv(dbimg_src_color_middle, hbins, sbins);
							}

						Mat dmimg_descriptor;
						if (useFea)
							dmimg_descriptor = calSURFDescriptor_one(dbimg_gray, minH);
						// ===== HSV =====
						for each (int i in valid_indexs) {
							double hsv_score = 0;
							double hsv_score_2 = 0;
							double hsv_score_3 = 0;
							double hsv_score_4 = 0;
							double hsv_score_D = 0;
							double hsv_score_MO = 0;

							if (useHsv) {
								// divided overall
								for (int j = 0; j < dbimg_color_divide_hsv.size(); j++) {
									hsv_score_D += compareHist(src_color_divide_hsv_[i][j], dbimg_color_divide_hsv[i], CV_COMP_CORREL);
								}
								if (dbimg_color_divide.size() > 0)
									hsv_score_D /= dbimg_color_divide.size();

								// middle 
								hsv_score_MO = compareHist(src_middle_hsv_[i], dbimg_hsv_middle, CV_COMP_CORREL);

								if (dbimg_color_divide.size() > 0) {
									hsv_score = hsv_score_D * 0.5 + hsv_score_MO  * 0.5;
									hsv_score_2 = hsv_score_D * 0.25 + hsv_score_MO  * 0.75;
									hsv_score_3 = hsv_score_D * 0.75 + hsv_score_MO  * 0.25;
									hsv_score_4 = hsv_score_D;
								} else
									hsv_score = 0;

							}

							//cout << "hsv_score:" << hsv_score << endl;
							//fea
							//vector<Mat> dbimg_fea = divide_image(dbimg_src, 4, 4);
							//dbimg_fea.erase(dbimg_fea.begin(), dbimg_fea.begin() + 5);
							//dbimg_fea.erase(dbimg_fea.begin() + 2, dbimg_fea.begin() + 4);
							//dbimg_fea.resize(4);

							// ===== Feature =====
							double fea_score = 0;
							double fea_score_2 = 0;
							double fea_score_3 = 0;
							double fea_score_4 = 0;

							double fea_score_MD = 0;
							double fea_score_MO = 0;

							if (useFea) {


								// ===== Middle Divide Descriptor =====

								for (int j = 0; j < src_d_descriptors_[i].size(); j++) {
									fea_score_MD += 1 - feature_cmp(src_d_descriptors_[i][j], dmimg_descriptor);
								}
								//								for (int i = 0; i < src_d_descriptors.size(); i++) {
								//									fea_score_MD += 1 - feature_cmp(src_d_descriptors[i], dmimg_descriptor);
								//>>>>>>> origin/master
								//								}
								if (src_d_descriptors_[i].size() > 0)
									fea_score_MD /= src_d_descriptors_[i].size();

								// ===== Middle Overall Descriptor =====
								fea_score_MO += (1 - feature_cmp(src_g_m_descriptor_[i], dmimg_descriptor));
								//=======
								//								fea_score_MO += (1 - feature_cmp(src_g_m_descriptor, dmimg_descriptor));
								//>>>>>>> origin/master

								if (src_d_descriptors_[i].size() > 0) {
									fea_score = fea_score_MD *0.5 + fea_score_MO * 0.5;
									fea_score_2 = fea_score_MD * 0.25 + fea_score_MO * 0.75;
									fea_score_3 = fea_score_MD * 0.75 + fea_score_MO * 0.25;
									fea_score_4 = fea_score_MD;
								} else
									fea_score = fea_score_MO;
							}
							//for (int i = 0; i < input_descriptors.size(); i++) {
							// fea_score += feature_cmp(input_descriptors[i], dmimg_descriptor);
							//}
							//fea_score /= (double) input_descriptors.size() * (input_descriptors.size() + 1) / 2;
							//fea_score /= input_descriptors.size();
							//cout << "fea_score:" << fea_score << endl;
							vector<double> calScore_list(15);
							if (useHsv && useFea) {
								calScore_list[0] = (hsv_score + 1) / 2 + (fea_score * 4);
								calScore_list[1] = (hsv_score + 1) / 2 + (fea_score * 8); // for bus
								calScore_list[2] = (hsv_score + 1) / 2 + (fea_score * 1);
								calScore_list[3] = (hsv_score + 1) / 2 + (fea_score * 0.5);

								calScore_list[4] = (hsv_score + 1) / 2 + (fea_score_2 * 4);
								calScore_list[5] = (hsv_score + 1) / 2 + (fea_score_3 * 4);

								calScore_list[6] = (hsv_score_2 + 1) / 2 + (fea_score * 1);

								/*
								hsv_score = hsv_score_D * 0.5 + hsv_score_MO  * 0.5;
								hsv_score_2 = hsv_score_D * 0.25 + hsv_score_MO  * 0.75;
								hsv_score_3 = hsv_score_D * 0.75 + hsv_score_MO  * 0.25;
								hsv_score_4 = hsv_score_D;
								*/
								/*
								fea_score = fea_score_MD *0.5 + fea_score_MO * 0.5;
								fea_score_2 = fea_score_MD * 0.25 + fea_score_MO * 0.75;
								fea_score_3 = fea_score_MD * 0.75 + fea_score_MO * 0.25;
								fea_score_4 = fea_score_MD;

								horse : 6, 8, 10, 12, 15
								*/
								calScore_list[7] = (hsv_score_3 + 1) / 2 + (fea_score * 1); /**/
								calScore_list[8] = (hsv_score_4 + 1) / 2 + (fea_score * 1);

								calScore_list[9] = (hsv_score + 1) / 2 + (fea_score_4 * 1); //
								calScore_list[10] = (hsv_score_2 + 1) / 2 + (fea_score_4 * 1);
								calScore_list[11] = (hsv_score_3 + 1) / 2 + (fea_score_4 * 1); //**//
								calScore_list[12] = (hsv_score_4 + 1) / 2 + (fea_score_4 * 1);

								calScore_list[13] = (hsv_score + 1) / 2 + (fea_score_4 * 0.5); // beach , building
								calScore_list[14] = (hsv_score + 1) / 2 + (fea_score_4 * 2);

								// fea_score_4
								for (int k = 0; k < calScore_list.size(); k++) {
									iss_list_[i][k].push_back(ImgScore(fileIndex, calScore_list[k]));
								}

							} else if (useHsv) {
								//calScore = hsv_score;
							} else if (useFea) {
								//calScore = fea_score;
							}
						}// each valid_index

					} catch (Exception e) {
						printf(" -----ERROR-----img#%i, %s\n", fileIndex, e.msg);
						errorIndex.push_back(fileIndex);
					}

					if (fileIndex % 50 == 0)
						printf("%i%% ", (fileIndex + 1) / 10);
				}
				cout << endl;

				//// compare hsv
				//Mat src_hsv = rgbMat_to_hsvHist(src_input);

				//for (int i = 0; i < features.size(); ++i)
				//	imgScoreResult.push_back(ImgScore(i, compareHist(features[i], src_hsv, 0)));

				reportImgError(errorIndex);

				ScoreReport sr;
				// ===== sort method 1 // for hsv? , or (1 - fea)
				for each (int i in valid_indexs) {
					for (int k = 0; k < iss_list_[i].size(); k++) {
						sort(iss_list_[i][k].rbegin(), iss_list_[i][k].rend());
						sr = ScoreReport(iss_list_[i][k], i);
						bestMData_[i].update(sr, k + 1, hbins, sbins, minH);
					}
				}
				//// ===== sort method 3
				//for (ImgScore &is : iss) {
				//	is.score = abs(is.score);
				//}
				//sort(iss.begin(), iss.end());
				//sr = ScoreReport(iss, inputIndex);
				//bestMData.update(sr, 3, hbins, sbins, minH);

				//// ===== sort method 4
				//sort(iss.rbegin(), iss.rend());
				//sr = ScoreReport(iss, inputIndex);
				//bestMData.update(sr, 4, hbins, sbins, minH);

				printf(" - ===== End of minHessian : %i =====\n", minH);
				printf(" - ===== End of hbin : %i , sbin: %i \n", hbins, sbins);
				for each (int i in valid_indexs) {
					bestMData_[i].report();
				}
				if (!useHsv) { goto fea_only; }
			} // sbins
		} // hbins
	fea_only:
		if (!useFea) { break; }
	} // minH

	//} // each input index

	for each (int valid_index in valid_indexs) {
		printf(" ===== Best of %s =====\n", files[valid_index].c_str());
		bestMData_[valid_index].report();
	}
}

/*
man		: 0 ~ 99
beach	: 101 ~ 199
building	: 200 ~ 299
bus		: 300 ~ 399
dinosaur	: 400 ~ 499
elephant	: 500 ~ 599
flower	: 600 ~ 699
horse	: 700 ~ 799
mountain	: 800 ~ 899
food	: 900 ~ 999
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


				//double score = feature_cmp(descriptors_1, descriptors_dbimg);
				double score = 0;
				for (int i = 0; i < busPreFeature.size(); i++) {
					score += feature_cmp(precut_descriptors[i], descriptors_dbimg);
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
//	printf("Error loading images\n"); return -1;
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
//	keyValue = cvWaitKey(0);
//
//	switch (keyValue) {
//		case 27:keyValue = -1;
//		break;
//	}
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
//	vector<Mat> splited_img = rgbMat_to_divided_hsvHist(imgs[i], row, col);
//	double scores = 0;
//	double sums = 0;
//	for (int i = 0; i < tot; ++i)
//		sums += compareHist(splited_img[i], src_hsv[i], 0);
//	imgScoreResult.push_back(ImgScore(i, sums / (double)(tot)));
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
