#include "feature_extract.h"
#include "compare.h"
#include "ScoreReport.h"
#include <ctime>
#include <opencv2/imgproc/imgproc.hpp>
#include <numeric> 
#include "method_report.h"

float getHistogramBinValue(Mat hist, int binNum) {
		return hist.at<float>(binNum);
}
float getFrequencyOfBin(Mat channel) {
		int histSize = 255;
		float frequency = 0.0;
		for (int i = 1; i < histSize; i++) {
				float Hc = abs(getHistogramBinValue(channel, i));
				frequency += Hc;
		}
		return frequency;
}
float computeShannonEntropy(Mat bgr_input) {
		// Mat ch1, ch2, ch3;
		Mat r, g, b;
		// "channels" is a vector of 3 Mat arrays:
		vector<Mat> channels(3);
		// split img:
		split(bgr_input, channels);
		// get the channels (dont forget they follow BGR order in OpenCV)
		b = channels[0];
		g = channels[1];
		r = channels[2];

		int histSize = 255;
		float entropy = 0.0;
		float frequency = getFrequencyOfBin(r);
		for (int i = 1; i < histSize; i++) {
				float Hc = abs(getHistogramBinValue(r, i));
				entropy += -(Hc / frequency) * log10((Hc / frequency));
		}
		frequency = getFrequencyOfBin(g);
		for (int i = 1; i < histSize; i++) {
				float Hc = abs(getHistogramBinValue(g, i));
				entropy += -(Hc / frequency) * log10((Hc / frequency));
		}
		frequency = getFrequencyOfBin(b);
		for (int i = 1; i < histSize; i++) {
				float Hc = abs(getHistogramBinValue(b, i));
				entropy += -(Hc / frequency) * log10((Hc / frequency));
		}
		entropy = entropy;
		cout << entropy << endl;
		return entropy;
}

Mat im_so_awsome(Mat src) {
		Mat dst;
		vector<Mat> channels;
		char* source_window = "Source image";
		char* equalized_window = "Equalized Image";

		/// Convert to grayscale
		//cvtColor(src, src, CV_BGR2GRAY);
		cvtColor(src, dst, CV_BGR2YCrCb); //change the color image from BGR to YCrCb format

		split(dst, channels); //split the image into channels

		equalizeHist(channels[0], channels[0]); //equalize histogram on the 1st channel (Y)

		merge(channels, dst); //merge 3 channels including the modified 1st channel into one image

		cvtColor(dst, dst, CV_YCrCb2BGR); //change the color image from YCrCb to BGR format (to display image properly)

		/// Apply Histogram Equalization
		// equalizeHist(src, dst);

		/// Display results
		if (!true == true) {
				namedWindow(source_window, CV_WINDOW_AUTOSIZE);
				namedWindow(equalized_window, CV_WINDOW_AUTOSIZE);

				imshow(source_window, src);
				imshow(equalized_window, dst);

				/// Wait until user exits the program
				waitKey(0);
		}
		return dst;
}

Mat toHigherContrast(Mat input, double alpha = 1.0) {
		/// Read image given by user
		Mat image = input;
		Mat new_image = Mat::zeros(image.size(), image.type());

		//double alpha = 1; /**< Simple contrast control */
		int beta = 0;
		/// Initialize values
		//std::cout << " Basic Linear Transforms " << std::endl;
		//std::cout << "-------------------------" << std::endl;
		//std::cout << "* Enter the alpha value [1.0-3.0]: "; std::cin >> alpha;
		//std::cout << "* Enter the beta value [0-100]: "; std::cin >> beta;

		/// Do the operation new_image(i,j) = alpha*image(i,j) + beta
		for (int y = 0; y < image.rows; y++) {
				for (int x = 0; x < image.cols; x++) {
						for (int c = 0; c < 3; c++) {
								new_image.at<Vec3b>(y, x)[c] =
										saturate_cast<uchar>(alpha*(image.at<Vec3b>(y, x)[c]) + beta);
						}
				}
		}

		return new_image;

		/// Create Windows
		//namedWindow("Original Image", 1);
		//namedWindow("New Image", 1);

		///// Show stuff
		//imshow("Original Image", image);
		//imshow("New Image", new_image);

		/// Wait until user press some key
		//waitKey();
}

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
		on_double_compare();
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
		int sbins_[3] = { 28, 28, 4 };
		int minH_[3] = { 128, 128, 32 };
		int hsv_divide_[2] = { 4, 4 };
		int fea_divide_[2] = { 7, 7 };

		double higherContrast_src = 1.2;
		double higherContrast_dbimg = 1.2;
		int method_count = METHOD_COUNT;

		int hce_method_src = 0;
		//int hce_method_dbimg = 0;

		bool useHsv = true, useFea = true; // not ready 

		vector<BestMethodData> bestMData_(8);

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

eachInputSrc:
		for each (int valid_index in valid_indexs) {

				string src = "./" + files[valid_index] + ".jpg";
				Mat src_color = imread(src.c_str(), CV_LOAD_IMAGE_COLOR);

				// 1: hc
				// 2: e
				// 3: hc>e
				// 4: e>hc
				switch (hce_method_src) {
						case 1:
								src_color = toHigherContrast(src_color, higherContrast_src);
								break;
						case 3:
								src_color = toHigherContrast(src_color, higherContrast_src);
								src_color = im_so_awsome(src_color);
								break;

						case 2:
								src_color = im_so_awsome(src_color);
								break;
						case 4:
								src_color = toHigherContrast(src_color, higherContrast_src);
								break;

						default:
								break;
				}


				Mat src_gray; // imread(src.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
				//src_gray = Mat(src_color,CV_L)
				cv::cvtColor(src_color, src_gray, CV_BGR2GRAY); // CV_BGR2GRAY


				Mat src_color_middle = cutMiddle(src_color);
				Mat src_gray_middle = cutMiddle(src_gray);

				src_color_[valid_index] = src_color;

				src_gray_[valid_index] = src_gray;
				src_color_middle_[valid_index] = src_color_middle;
				src_gray_middle_[valid_index] = src_gray_middle;
		}

eachSetting:
		vector<double> times_need;
		vector<MethodResult> list_of_method_result;

		//bestMethodData bestMData;
		for (int hce_method_dbimg = 1; hce_method_dbimg <= 1; hce_method_dbimg++) {
				for (int minH = minH_[0]; minH <= minH_[1]; minH += minH_[2]) {
						for (int hbins = hbins_[0]; hbins <= hbins_[1]; hbins += hbins_[2]) {
								for (int sbins = sbins_[0]; sbins <= sbins_[1]; sbins += sbins_[2]) {

										using namespace std;
										clock_t begin = clock();


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
										vector<Mat> src_g_descriptor_(8);
										vector<Mat> src_g_m_descriptor_(8);
										vector<vector<Mat>> src_d_descriptors_(8);
										if (useFea) {
												for each (int i in valid_indexs) {
														src_g_descriptor_[i] = calSURFDescriptor_one(src_gray_[i], minH);
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
												iss_list_[i] = vector<vector<ImgScore>>(method_count);
										}

										// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
										// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
										// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
										for (int fileIndex = 0; fileIndex < 1000; fileIndex++) {
												try {
														// ===== Load file =====
														string file = "../image.orig/" + to_string(fileIndex) + ".jpg";

														Mat dbimg_color = imread(file.c_str(), CV_LOAD_IMAGE_COLOR);
														// 1: hc
														// 2: e
														// 3: hc>e
														// 4: e>hc
														switch (hce_method_dbimg) {
																case 1:
																		dbimg_color = toHigherContrast(dbimg_color, higherContrast_src);
																		break;
																case 3:
																		dbimg_color = toHigherContrast(dbimg_color, higherContrast_src);
																		dbimg_color = im_so_awsome(dbimg_color);
																		break;

																case 2:
																		dbimg_color = im_so_awsome(dbimg_color);
																		break;
																case 4:
																		dbimg_color = im_so_awsome(dbimg_color);
																		dbimg_color = toHigherContrast(dbimg_color, higherContrast_src);
																		break;

																default:
																		break;
														}




														//Mat dbimg_gray = imread(file.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
														Mat dbimg_gray;
														cv::cvtColor(dbimg_color, dbimg_gray, CV_BGR2GRAY); // CV_BGR2GRAY

														//dbimg_gray = toHigherContrast(dbimg_gray, higherContrast);

														Mat dbimg_src_color_middle = cutMiddle(dbimg_color);
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
																double hsv_score_4_D = 0;
																double hsv_score_5_MOA = 0;

																double hsv_score_D_only = 0;
																double hsv_score_Middle_Overall = 0;

																if (useHsv) {
																		// divided overall
																		for (int j = 0; j < dbimg_color_divide_hsv.size(); j++) {
																				hsv_score_D_only += compareHist(src_color_divide_hsv_[i][j], dbimg_color_divide_hsv[i], CV_COMP_CORREL);
																		}
																		if (dbimg_color_divide.size() > 0)
																				hsv_score_D_only /= dbimg_color_divide.size();

																		// middle 
																		hsv_score_Middle_Overall = compareHist(src_middle_hsv_[i], dbimg_hsv_middle, CV_COMP_CORREL);

																		if (dbimg_color_divide.size() > 0) {
																				hsv_score = hsv_score_D_only * 0.5 + hsv_score_Middle_Overall  * 0.5;
																				hsv_score_2 = hsv_score_D_only * 0.25 + hsv_score_Middle_Overall  * 0.75;
																				hsv_score_3 = hsv_score_D_only * 0.75 + hsv_score_Middle_Overall  * 0.25;
																				hsv_score_4_D = hsv_score_D_only;
																				hsv_score_5_MOA = hsv_score_Middle_Overall;
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
																double fea_score_4_MD = 0;
																double fea_score_5_OA = 0;
																double fea_score_6_MOA = 0;

																double fea_score_Overall = 0;
																double fea_score_Middle_D = 0;
																double fea_score_Middle_Overall = 0;

																if (useFea) {
																		// ===== ===== ===== ===== Descriptor ===== ===== ===== =====
																		// ===== ===== ===== ===== Descriptor ===== ===== ===== =====
																		// ===== ===== ===== ===== Descriptor ===== ===== ===== =====
																		// ===== ===== ===== ===== Descriptor ===== ===== ===== =====
																		// ===== ===== ===== ===== Descriptor ===== ===== ===== =====
																		// ===== Middle Divide Descriptor =====
																		fea_score_Overall = (1 - feature_cmp(src_g_descriptor_[i], dmimg_descriptor));

																		for (int j = 0; j < src_d_descriptors_[i].size(); j++) {
																				fea_score_Middle_D += 1 - feature_cmp(src_d_descriptors_[i][j], dmimg_descriptor);
																		}



																		//								for (int i = 0; i < src_d_descriptors.size(); i++) {
																		//									fea_score_MD += 1 - feature_cmp(src_d_descriptors[i], dmimg_descriptor);
																		//>>>>>>> origin/master
																		//								}
																		if (src_d_descriptors_[i].size() > 0)
																				fea_score_Middle_D /= src_d_descriptors_[i].size();

																		// ===== Middle Overall Descriptor =====
																		fea_score_Middle_Overall += (1 - feature_cmp(src_g_m_descriptor_[i], dmimg_descriptor));
																		//=======
																		//								fea_score_MO += (1 - feature_cmp(src_g_m_descriptor, dmimg_descriptor));
																		//>>>>>>> origin/master

																		if (src_d_descriptors_[i].size() > 0) {
																				fea_score = fea_score_Middle_D *0.5 + fea_score_Middle_Overall * 0.5;
																				fea_score_2 = fea_score_Middle_D * 0.25 + fea_score_Middle_Overall * 0.75;
																				fea_score_3 = fea_score_Middle_D * 0.75 + fea_score_Middle_Overall * 0.25;
																				fea_score_4_MD = fea_score_Middle_D;
																				fea_score_5_OA = fea_score_Overall;
																				fea_score_6_MOA = fea_score_Middle_Overall;
																		} else
																				fea_score = fea_score_Middle_Overall;
																}
																//for (int i = 0; i < input_descriptors.size(); i++) {
																// fea_score += feature_cmp(input_descriptors[i], dmimg_descriptor);
																//}
																//fea_score /= (double) input_descriptors.size() * (input_descriptors.size() + 1) / 2;
																//fea_score /= input_descriptors.size();
																//cout << "fea_score:" << fea_score << endl;
																vector<double> calScore_list(method_count);
																if (useHsv && useFea) {
																		/*
																		hsv_score = hsv_score_D * 0.5 + hsv_score_MO  * 0.5;
																		hsv_score_2 = hsv_score_D * 0.25 + hsv_score_MO  * 0.75;
																		hsv_score_3 = hsv_score_D * 0.75 + hsv_score_MO  * 0.25;

																		hsv_score_4_D = hsv_score_D_only;
																		hsv_score_5_MOA = hsv_score_Middle_Overall;
																		*/

																		/*
																		fea_score = fea_score_MD *0.5 + fea_score_MO * 0.5;
																		fea_score_2 = fea_score_MD * 0.25 + fea_score_MO * 0.75;
																		fea_score_3 = fea_score_MD * 0.75 + fea_score_MO * 0.25;

																		fea_score_4_MD = fea_score_Middle_D;
																		fea_score_5_OA = fea_score_Overall;
																		fea_score_6_MOA = fea_score_Middle_Overall;

																		horse : 6, 8, 10, 12, 15
																		*/
																		calScore_list[0] = (hsv_score + 1) / 2 + (fea_score * 4);
																		calScore_list[1] = (hsv_score + 1) / 2 + (fea_score * 8); // for bus

																		calScore_list[2] = (hsv_score + 1) / 2 + (fea_score * 1);
																		calScore_list[3] = (hsv_score + 1) / 2 + (fea_score * 0.5);

																		calScore_list[4] = (hsv_score + 1) / 2 + (fea_score_2 * 4);
																		calScore_list[5] = (hsv_score + 1) / 2 + (fea_score_3 * 4);

																		calScore_list[6] = (hsv_score_2 + 1) / 2 + (fea_score * 1);
																		calScore_list[7] = (hsv_score_3 + 1) / 2 + (fea_score * 1); /**/

																		//calScore_list[8] = (hsv_score_4 + 1) / 2 + (fea_score * 1);
																		calScore_list[8] = (hsv_score_5_MOA + 1) / 2 + (fea_score_4_MD * 1); // new

																		calScore_list[9] = (hsv_score + 1) / 2 + (fea_score_4_MD * 1); //

																		//	hsv_score_2 = hsv_score_D * 0.25 + hsv_score_MO  * 0.75;
																		// fea_score_4 = fea_score_MD;
																		calScore_list[10] = (hsv_score_2 + 1) / 2 + (fea_score_4_MD * 1);
																		calScore_list[11] = (hsv_score_3 + 1) / 2 + (fea_score_4_MD * 1); //**//
																		calScore_list[12] = (hsv_score_4_D + 1) / 2 + (fea_score_4_MD * 1); //<< suck

																		calScore_list[13] = (hsv_score + 1) / 2 + (fea_score_4_MD * 0.5); // beach , building
																		calScore_list[14] = (hsv_score + 1) / 2 + (fea_score_4_MD * 2);

																		calScore_list[15] = (hsv_score_5_MOA + 1) / 2 + (fea_score * 2);
																		calScore_list[16] = (hsv_score + 1) / 2 + (fea_score_5_OA * 2);
																		calScore_list[17] = (hsv_score + 1) / 2 + (fea_score_6_MOA * 2);

																		calScore_list[18] = (hsv_score + 1) / 2 + (fea_score_5_OA * 0.75 + fea_score_6_MOA *0.25);
																		calScore_list[19] = (hsv_score + 1) / 2 + (fea_score_5_OA * 0.5 + fea_score_6_MOA *0.5);

																		calScore_list[20] = (hsv_score_4_D + 1) / 2 + (fea_score_5_OA * 0.75 + fea_score_6_MOA *0.25);
																		calScore_list[21] = (hsv_score_4_D + 1) / 2 + (fea_score_5_OA * 0.5 + fea_score_6_MOA *0.5);

																		calScore_list[22] = (hsv_score_5_MOA + 1) / 2 + (fea_score_5_OA * 0.75 + fea_score_6_MOA *0.25);
																		calScore_list[23] = (hsv_score_5_MOA + 1) / 2 + (fea_score_5_OA * 0.5 + fea_score_6_MOA *0.5);

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
														cout << "method#" << k + 1 << " : ";
														bestMData_[i].update(sr, k + 1, hbins, sbins, minH);
												}
										}

										printf(" - ===== End of minHessian : %i =====\n", minH);
										printf(" - ===== End of hbin : %i , sbin: %i =====\n\n", hbins, sbins);
										for each (int i in valid_indexs) {
												printf(" ===== so far Best of %s =====\n", files[i].c_str());
												bestMData_[i].report();
										}

										MethodResult m_result;
										m_result._hbins = hbins;
										m_result._sbins = sbins;
										m_result._minH = minH;
										m_result._hce_method_dbimg = hce_method_dbimg;

										// acc and firstMatch count of each method
										vector<double> acc_of_method_;
										vector<double> acc_first_match_;
										vector<int> match_count_method;

										for (int k = 0; k < method_count; k++) {
												double acc = 0;
												double acc_first_match = 0;
												int count = 0;
												int match_count = 0;

												vector<ScoreReport> for_best_threshold;

												for each (int i in valid_indexs) {
														acc += bestMData_[i].srs[k].acc100;
														count++;
														for_best_threshold.push_back(bestMData_[i].srs[k]);
														// need firstMatch count
														if (bestMData_[i].srs[k].firstWrong != 0) {
																match_count++;
																acc_first_match += bestMData_[i].srs[k].acc100;
														}
												}
												acc /= 7;
												if (match_count > 0)
														acc_first_match /= match_count;

												acc_of_method_.push_back(acc);
												acc_first_match_.push_back(acc_first_match);

												match_count_method.push_back(match_count);
												m_result.add_best_threshold(for_best_threshold);

												//string fname = "./method/method" + to_string(k + 1) + ".yml";
												//FileStorage fs(fname.c_str(), FileStorage::WRITE);
												//write(fs, "ScoreReport", for_best_threshold);
												//fs.release();


										} //each method
										m_result._acc_of = acc_of_method_;
										m_result._acc_first_match = acc_first_match_;
										m_result._first_match_count = match_count_method;

										m_result.report();
										list_of_method_result.push_back(m_result);

										clock_t end = clock();
										double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
										times_need.push_back(elapsed_secs);
										printf("\nused time:%f\n", elapsed_secs);


										// show best method:
										double bestMethodAcc = 0;
										MethodResult bestMR;
										for each (MethodResult mr in list_of_method_result) {
												for each (double acc in mr._acc_of) {
														if (acc > bestMethodAcc) {
																bestMethodAcc = acc;
																bestMR = mr;
														}
												}
										}

										printf(" ===== so far: Best Acc in: =====\n");
										bestMR.report();


										if (!useHsv) { goto fea_only; }
								} // sbins
						} // hbins
				fea_only:
						if (!useFea) { break; }
				} // minH
		}// hce_method_dbimg

		double average = accumulate(times_need.begin(), times_need.end(), 0.0) / times_need.size();

		// after all:
		//double bestMethodAcc = 0;
		//MethodResult bestMR;
		//for each (MethodResult mr in list_of_method_result) {
		//		for each (double acc in mr._acc_of) {
		//				if (acc > bestMethodAcc) {
		//						bestMethodAcc = acc;
		//						bestMR = mr;
		//				}
		//		}
		//}

		//printf(" ===== Best Acc in: =====\n");
		//bestMR.report();


		//} // each input index

		//for each (int vi in valid_indexs) {
		//		printf(" ===== Best of %s =====\n", files[vi].c_str());
		//		bestMData_[vi].report();
		//}
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
//void double_compare_bus() {
//		// Mat src_input, vector<vector<Mat>> features, int inputIndex
//		// Mat bus_1 = imread("./precut/bus_1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//		// Mat bus_1 = imread("./precut/bus_3.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//		// Mat bus_1 = imread("./bus.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//
//		// imread("./precut/bus_1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//		// Mat bus_2 = imread("./precut/bus_2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//
//		vector<Mat> busPreFeature = {
//				imread("./precut/bus_1.jpg", CV_LOAD_IMAGE_GRAYSCALE)
//				, imread("./precut/bus_3.jpg", CV_LOAD_IMAGE_GRAYSCALE)
//		};
//
//
//		bool showImg = false;
//		int start = 0, end = 1000;
//		int minHessian;
//
//		double bestAcc = 0;
//		int bestMinH = 0;
//		int useCase = 0;
//
//		for (int j = 0; j < 100; j++) {
//				minHessian = 4 + 4 * j;
//
//				SurfFeatureDetector detector(minHessian);
//				SurfDescriptorExtractor extractor;
//
//				vector<std::vector<KeyPoint>> precut_keypoints(busPreFeature.size());
//				vector<Mat> precut_descriptors(busPreFeature.size());
//
//				for (int i = 0; i < busPreFeature.size(); i++) {
//						detector.detect(busPreFeature[i], precut_keypoints[i]);
//						extractor.compute(busPreFeature[i], precut_keypoints[i], precut_descriptors[i]);
//				}
//
//
//				//std::vector<KeyPoint> keypoints_1, keypoints_2;
//				//detector.detect(bus_1, keypoints_1);
//
//				std::vector<KeyPoint>keypoints_2;
//
//				Mat descriptors_dbimg;
//
//
//
//				vector<ImgScore> iss;
//				vector<int> eIndex;
//				for (int i = start; i < end; i++) {
//						try {
//								string file = "../image.orig/" + to_string(i) + ".jpg";
//								Mat db_img = imread(file.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
//
//								if (!db_img.data) {
//										printf("Error loading images\n"); return;
//								}
//
//								//vector<Mat> SURFDescriptors;
//								//SURFDescriptors.push_back(calSURFDescriptor(bus_1));
//								//SURFDescriptors.push_back(calSURFDescriptor(bus_2));
//
//								//-- Step 1: Detect the keypoints using SURF Detector
//								detector.detect(db_img, keypoints_2);
//
//								//-- Step 2: Calculate descriptors (feature vectors)
//								extractor.compute(db_img, keypoints_2, descriptors_dbimg);
//
//								//-- Step 3: Matching descriptor vectors with a brute force matcher
//								BFMatcher matcher(NORM_L2);
//								// BFMatcher matcher(NORM_HAMMING);
//								// FlannBasedMatcher matcher;
//
//								std::vector<DMatch> matches;
//								for (int i = 0; i < busPreFeature.size(); i++) {
//										//matcher.match(descriptors_1, descriptors_dbimg, matches);
//										matcher.match(precut_descriptors[i], descriptors_dbimg, matches);
//								}
//
//
//								//-- Draw matches
//
//								if (showImg) {
//										Mat img_matches;
//										for (int i = 0; i < busPreFeature.size(); i++) {
//												drawMatches(busPreFeature[i], precut_keypoints[i], db_img, keypoints_2, matches, img_matches);
//
//												//drawMatches(bus_1, keypoints_1, db_img, keypoints_2, matches, img_matches);
//												imshow("Matches", img_matches);
//
//												int keyValue = 0;
//												while (keyValue >= 0) {
//														keyValue = cvWaitKey(0);
//
//														switch (keyValue) {
//																case 27:keyValue = -1;
//																		break;
//														}
//												}
//										}
//								}
//
//
//								//double score = feature_cmp(descriptors_1, descriptors_dbimg);
//								double score = 0;
//								for (int i = 0; i < busPreFeature.size(); i++) {
//										score += feature_cmp(precut_descriptors[i], descriptors_dbimg);
//								}
//								score /= busPreFeature.size();
//								iss.push_back(ImgScore(i, score));
//								//printf(" img#%i, match score:%f \n", i, score);
//								if (i % 10 == 0)
//										printf(" %i%%..", i / 10);
//								//-- Show detected matches
//								// imshow("Matches", img_matches);
//								// imshow("bus_1", bus_1);
//						} catch (Exception e) {
//								printf(" -----ERROR-----img#%i, %s\n", i, e.msg);
//								eIndex.push_back(i);
//						}
//				}
//				printf("\n\n");
//
//				// Sort
//				sort(iss.begin(), iss.end());
//
//				reportImgError(eIndex);
//				printf("-----\n");
//
//				ScoreReport sr(iss, 3);
//				sr.reportSorted(300);
//				sr.report();
//
//
//				printf("=====End of minHessian=%3i\n=====\n", minHessian);
//				//cout << "acc:" << sr.acc100 <<" , bestAcc" << bestAcc;
//				if (sr.acc100 > bestAcc) {
//						bestAcc = sr.acc100;
//						bestMinH = minHessian;
//				}
//				printf("-Current best minH:%i, Acc100:%.0f", bestMinH, bestAcc);
//		}// for j
//		printf("best minH=%i Acc: %.4f%% | ", bestMinH, bestAcc);
//
//		printf("-1 to exit");
//		int input;
//		do {
//				cin >> input;
//		} while (input != -1);
//
//
//}


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
