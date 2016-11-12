#include "feature_extract.h"
#include "compare.h"
#include "ScoreReport.h"
#include <ctime>
#include <opencv2/imgproc/imgproc.hpp>
#include <numeric> 
#include "method_report.h"
#include <opencv2/core/core.hpp>
#include <iostream>
static void write(FileStorage& fs, const std::string&, const ImgScore& x) {
	x.write(fs);
}
static void read(const FileNode& node, ImgScore& x, const ImgScore& default_value = ImgScore()) {
	if (node.empty())
		x = default_value;
	else
		x.read(node);
}

static ostream& operator<<(ostream& out, const ImgScore& m) {
	out << "{ id = " << m.db_id << ", ";
	out << "score = " << m.score << "}";
	return out;
}

void test_read_backup() {
	int valid_indexs[] = { 0, 1, 2, 3, 4, 6, 7 }; // 0, 1, 2, 3, 4, 6, 7

	vector<ImgScore> save_list;
	FileStorage fs2("./core_result/input_6.yml", FileStorage::READ);
	FileNode kptFileNode = fs2["ImgScore"];
	read(kptFileNode, save_list);
	fs2.release();

	ScoreReport sr(save_list, 6);
	sr.report();
}

void test_read() {
	int valid_indexs[] = { 0, 1, 2, 3, 4, 6, 7 }; // 0, 1, 2, 3, 4, 6, 7
	int skip_method[] = { 0 };
	MethodResult mr;
	for (int method = 0; method < METHOD_COUNT; method++) {
		if (method != 5)
			continue;

		cout << "doing method:" << method + 1 << "...";

		vector<ScoreReport> sr_list;
		for each(int i in valid_indexs) {

			vector<ImgScore> save_list;
			string  filename = "./test_result/method_" + to_string(method) + "input_" + to_string(i) + ".yml";
			FileStorage fs2(filename.c_str(), FileStorage::READ);
			FileNode kptFileNode = fs2["ImgScore"];
			read(kptFileNode, save_list);
			fs2.release();

			ScoreReport sr(save_list, i);
			sr_list.push_back(sr);

		}
		mr.sr_list.push_back(sr_list);
		mr.add_acc(sr_list);
		mr.add_best_threshold(sr_list);
	}
	cout << endl;
	mr.report();
}

void save_D_C_R(vector<ImgScore> scoreList, int inputIndex, int method) {
	vector<ImgScore> save_list = scoreList;
	string  filename = "./test_result/method_" + to_string(method) + "input_" + to_string(inputIndex) + ".yml";
	FileStorage fs(filename.c_str(), FileStorage::WRITE);
	//write(fs, "aNameYouLike", save_list);
	fs << "ImgScore" << "[";
	for each(ImgScore ims in save_list) { fs << ims; }
	fs << "]";   // close sequence
	fs.release();
}

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

Mat equalize_channel_Y(Mat src) {
	Mat dst;
	vector<Mat> channels;
	//char* source_window = "Source image";
	//char* equalized_window = "Equalized Image";

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
	//if (!true == true) {
	//	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	//	namedWindow(equalized_window, CV_WINDOW_AUTOSIZE);

	//	imshow(source_window, src);
	//	imshow(equalized_window, dst);

	//	/// Wait until user exits the program
	//	waitKey(0);
	//}
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

vector<int> new_compare(Mat src_color, int inputIndex, double threshold) {
	//int hbins_[3] = { 24, 24, 8 };
	//int sbins_[3] = { 28, 28, 4 };
	//int minH_[3] = { 128, 128, 32 };
	int hsv_divide_[2] = { 4, 4 };
	int fea_divide_[2] = { 5, 5 };
	int minH = 192, hbins = 24, sbins = 28;
	//int hce_method_src = 2 , hce_method_dbimg = 2;
	//double higherContrast_src = 1.2;
	//double higherContrast_dbimg = 1.2;
	//int method_count = METHOD_COUNT;

	Mat src_gray; // imread(src.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
	cv::cvtColor(src_color, src_gray, CV_BGR2GRAY); // CV_BGR2GRAY

	src_color = equalize_channel_Y(src_color);


	Mat src_color_middle = cutMiddle(src_color);
	Mat src_gray_middle = cutMiddle(src_gray);

	vector<double> times_need;
	vector<MethodResult> list_of_method_result;

	using namespace std;
	clock_t begin = clock();

	vector<Mat> src_color_divide;
	vector<Mat>	src_gray_divide;
	// ===== HSV =====
	Mat src_hsv;
	Mat src_middle_hsv;
	// ===== HSV with divide =====
	vector<Mat> src_color_divide_hsv;

	src_color_divide = divide_image(src_color, hsv_divide_[0], hsv_divide_[1]);
	src_gray_divide = divide_image(src_gray, fea_divide_[0], fea_divide_[1]);

	src_hsv = img_src_to_hsv(src_color, hbins, sbins);
	src_middle_hsv = img_src_to_hsv(src_color_middle, hbins, sbins);

	for (int j = 0; j < src_color_divide.size(); j++) {
		src_color_divide_hsv.push_back(img_src_to_hsv(src_color_divide[j], hbins, sbins));
	}

	// ===== Feature descriptor =====
	Mat src_g_descriptor;
	Mat src_g_m_descriptor;
	vector<Mat> src_d_descriptors;

	src_g_descriptor = calSURFDescriptor_one(src_gray, minH);
	src_g_m_descriptor = calSURFDescriptor_one(src_gray_middle, minH);

	for (int j = 0; j < src_gray_divide.size(); j++) {
		Mat temp_desc = calSURFDescriptor_one(src_gray_divide[j], minH);
		// it seems some img may not able to find any descriptor when it is too small
		// we just ignore this currently
		if (!temp_desc.empty())
			src_d_descriptors.push_back(temp_desc);
		else {
			//cout << "log:empty:" << j << endl;
		}
	}

	vector<int> errorIndex;

	vector<ImgScore> iss_list;
	for (int fileIndex = 0; fileIndex < 1000; fileIndex++) {
		try {
			// ===== Load file =====
			string file = "../image.orig/" + to_string(fileIndex) + ".jpg";

			Mat dbimg_color = imread(file.c_str(), CV_LOAD_IMAGE_COLOR);
			Mat dbimg_gray;
			cv::cvtColor(dbimg_color, dbimg_gray, CV_BGR2GRAY); // CV_BGR2GRAY

			dbimg_color = equalize_channel_Y(dbimg_color);

			Mat dbimg_src_color_middle = cutMiddle(dbimg_color);


			// ===== Divide (for HSV) =====
			vector<Mat> dbimg_color_divide = divide_image(dbimg_color, hsv_divide_[0], hsv_divide_[1]);
			vector<Mat> dbimg_color_divide_hsv;
			Mat dbimg_hsv_middle;

			for (int i = 0; i < dbimg_color_divide.size(); i++) {
				dbimg_color_divide_hsv.push_back(img_src_to_hsv(dbimg_color_divide[i], hbins, sbins));

				// hsv middle
				dbimg_hsv_middle = img_src_to_hsv(dbimg_src_color_middle, hbins, sbins);
			}

			Mat dmimg_descriptor; // we can store it
			dmimg_descriptor = calSURFDescriptor_one(dbimg_gray, minH);
			// ===== HSV =====

			double hsv_score = 0;
			//double hsv_score_2 = 0;
			//double hsv_score_3 = 0;
			//double hsv_score_4_D = 0;
			//double hsv_score_5_MOA = 0;

			double hsv_score_Divide_Only = 0;
			double hsv_score_Middle_Overall = 0;


			// divided overall
			for (int j = 0; j < dbimg_color_divide_hsv.size(); j++) {
				hsv_score_Divide_Only += compareHist(src_color_divide_hsv[j], dbimg_color_divide_hsv[j], CV_COMP_CORREL);
			}
			if (dbimg_color_divide.size() > 0)
				hsv_score_Divide_Only /= dbimg_color_divide.size();

			// middle 
			hsv_score_Middle_Overall = compareHist(src_middle_hsv, dbimg_hsv_middle, CV_COMP_CORREL);

			if (dbimg_color_divide.size() > 0) {
				hsv_score = hsv_score_Divide_Only * 0.5 + hsv_score_Middle_Overall  * 0.5;
				//hsv_score_2 = hsv_score_D_only * 0.25 + hsv_score_Middle_Overall  * 0.75;
				//hsv_score_3 = hsv_score_D_only * 0.75 + hsv_score_Middle_Overall  * 0.25;
				//hsv_score_4_D = hsv_score_D_only;
				//hsv_score_5_MOA = hsv_score_Middle_Overall;
			}

			// ===== Feature =====
			//double fea_score = 0;
			//double fea_score_2 = 0;
			double fea_score_3 = 0;
			//double fea_score_4_MD = 0;
			//double fea_score_5_OA = 0;
			//double fea_score_6_MOA = 0;

			//double fea_score_Overall = 0;
			double fea_score_Middle_Divide = 0;
			//double fea_score_Middle_Overall = 0;


			// ===== Middle Divide Descriptor =====
			//fea_score_Overall = (1 - feature_cmp(src_g_descriptor, dmimg_descriptor));

			for (int j = 0; j < src_d_descriptors.size(); j++) {
				fea_score_Middle_Divide += 1 - feature_cmp(src_d_descriptors[j], dmimg_descriptor);
			}
			if (src_d_descriptors.size() > 0)
				fea_score_Middle_Divide /= src_d_descriptors.size();

			// ===== Middle Overall Descriptor =====
			double fea_score_Middle_Overall = (1 - feature_cmp(src_g_m_descriptor, dmimg_descriptor));


			if (src_d_descriptors.size() > 0) {
				//fea_score = fea_score_Middle_D *0.5 + fea_score_Middle_Overall * 0.5;
				//fea_score_2 = fea_score_Middle_D * 0.25 + fea_score_Middle_Overall * 0.75;
				fea_score_3 = fea_score_Middle_Divide * 0.75 + fea_score_Middle_Overall * 0.25;
				//fea_score_4_MD = fea_score_Middle_D;
				//fea_score_5_OA = fea_score_Overall;
				//fea_score_6_MOA = fea_score_Middle_Overall;
			} else {
				//fea_score = fea_score_Middle_Overall;
			}

			double calScore = 0;

			calScore = (hsv_score + (double)1.0) / 2.0 + (fea_score_3 * 4.0);
			//calScore = (hsv_score_3 + 1) / 2 + (fea_score_4_MD * 1); //**//

			iss_list.push_back(ImgScore(fileIndex, calScore));




		} catch (Exception e) {
			printf(" -----ERROR-----img#%i, %s\n", fileIndex, e.msg);
			errorIndex.push_back(fileIndex);
		}

		if (fileIndex % 50 == 0)
			printf("%i%% ", (fileIndex + 1) / 10);
	}
	cout << endl;

	// sort
	ScoreReport sr;
	sort(iss_list.rbegin(), iss_list.rend());
	sr = ScoreReport(iss_list, inputIndex);
	sr.reportSorted(200);
	sr.report();

	// check time
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	times_need.push_back(elapsed_secs);
	printf("\nused time:%f\n", elapsed_secs);

	double input;
	cout << "using threshold " << threshold << " (-1 for ok, -2 to save, others value for change the threshold):";
	cin >> input;
	do {

		if (input != -1 && input != -2) {
			threshold = input;
		}

		vector<ImgScore> iss_threshold = iss_list;

		// apply threshold
		for (int i = 0; i < iss_threshold.size(); i++) {
			if (iss_threshold[i].score < threshold) {
				iss_threshold.resize(i);
				break;
			}
		}
		ScoreReport sr_threshold = ScoreReport(iss_threshold, inputIndex);


		double p = sr_threshold.correct / (double) iss_threshold.size() * (double) 100.0;
		double r = sr_threshold.correct;

		printf("p:%f, r:%f, size:%i (", p, r, iss_threshold.size());

		vector<int> res;
		for (int i = 0; i < iss_threshold.size(); ++i)
			res.push_back(iss_threshold[i].db_id);

		cout << "-1 for leave, -2 to save, others for try other threshold):";
		cin >> input;

		if (input == -2) {
			vector<ImgScore> save_list = sr.scoreList;
			string  filename = "./core_result/input_" + to_string(inputIndex) + ".yml";
			FileStorage fs(filename.c_str(), FileStorage::WRITE);
			//write(fs, "aNameYouLike", save_list);
			fs << "ImgScore" << "[";
			for each(ImgScore ims in save_list) { fs << ims; }
			fs << "]";   // close sequence
			fs.release();
		}

		if (input == -1)
			return res;
	} while (true);


}


void on_double_compare() {
declare:
	int debugIndex = -1;
	//int inputIndex = -1;
	int hbins_[3] = { 24, 24, 8 };
	int sbins_[3] = { 28, 28, 4 };
	int minH_[3] = { 192, 192, 32 };
	int hsv_divide_[2] = { 4, 4 };
	int fea_divide_[2] = { 5, 5 };

	double higherContrast_src = 1.2;
	double higherContrast_dbimg = 1.2;

	int hce_method_src = 2;
	int hce_method_dbimg = 2;

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
		Mat src_gray; // imread(src.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		//src_gray = Mat(src_color,CV_L)
		cv::cvtColor(src_color, src_gray, CV_BGR2GRAY); // CV_BGR2GRAY

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
				src_color = equalize_channel_Y(src_color);
				break;

			case 2:
				src_color = equalize_channel_Y(src_color);
				break;
			case 4:
				src_color = toHigherContrast(src_color, higherContrast_src);
				break;

			default:
				break;
		}





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
								//cout << "log:empty:" << i << ":" << j << endl;
							}
						}
					}

				}
				vector<int> errorIndex;

				vector<vector<vector<ImgScore>>> iss_list_(8);
				for (int i = 0; i < iss_list_.size(); i++) {
					iss_list_[i] = vector<vector<ImgScore>>(METHOD_COUNT);
				}

				// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
				// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
				// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
				for (int fileIndex = 0; fileIndex < 1000; fileIndex++) {
					try {
						// ===== Load file =====
						string file = "../image.orig/" + to_string(fileIndex) + ".jpg";

						Mat dbimg_color = imread(file.c_str(), CV_LOAD_IMAGE_COLOR);
						Mat dbimg_gray;
						cv::cvtColor(dbimg_color, dbimg_gray, CV_BGR2GRAY); // CV_BGR2GRAY
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
								dbimg_color = equalize_channel_Y(dbimg_color);
								break;

							case 2:
								dbimg_color = equalize_channel_Y(dbimg_color);
								break;
							case 4:
								dbimg_color = equalize_channel_Y(dbimg_color);
								dbimg_color = toHigherContrast(dbimg_color, higherContrast_src);
								break;

							default:
								break;
						}

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

								if (src_d_descriptors_[i].size() > 0)
									fea_score_Middle_D /= src_d_descriptors_[i].size();

								// ===== Middle Overall Descriptor =====
								fea_score_Middle_Overall += (1 - feature_cmp(src_g_m_descriptor_[i], dmimg_descriptor));

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
							vector<double> calScore_list(METHOD_COUNT);
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
								calScore_list[0] = (hsv_score + (double)1.0) / 2.0 + (fea_score * 4.0);
								calScore_list[1] = (hsv_score + (double)1.0) / 2.0 + (fea_score * 8.0); // for bus

								calScore_list[2] = (hsv_score + (double)1.0) / 2.0 + (fea_score * 1.0);
								calScore_list[3] = (hsv_score + (double)1.0) / 2.0 + (fea_score * 0.5);

								calScore_list[4] = (hsv_score + (double)1.0) / 2.0 + (fea_score_2 * 4.0);
								calScore_list[5] = (hsv_score + (double)1.0) / 2.0 + (fea_score_3 * 4.0);

								//calScore_list[6] = (hsv_score_2 + (double) 1.0) / 2.0 + (fea_score * 1.0);
								//calScore_list[7] = (hsv_score_3 + (double) 1.0) / 2.0 + (fea_score * 1.0); /**/

								////calScore_list[8] = (hsv_score_4 + (double) 1.0) / 2.0 + (fea_score * 1);
								//calScore_list[8] = (hsv_score_5_MOA + (double) 1.0) / 2.0 + (fea_score_4_MD * 1.0); // new

								//calScore_list[9] = (hsv_score + (double) 1.0) / 2.0 + (fea_score_4_MD * 1.0); //

								////	hsv_score_2 = hsv_score_D * 0.25 + hsv_score_MO  * 0.75;
								//// fea_score_4 = fea_score_MD;
								//calScore_list[10] = (hsv_score_2 + (double) 1.0) / 2.0 + (fea_score_4_MD * 1.0);
								//calScore_list[11] = (hsv_score_3 + (double) 1.0) / 2.0 + (fea_score_4_MD * 1.0); //**//
								//calScore_list[12] = (hsv_score_4_D + (double) 1.0) / 2.0 + (fea_score_4_MD * 1.0); //<< suck

								//calScore_list[13] = (hsv_score + (double) 1.0) / 2.0 + (fea_score_4_MD * 0.5); // beach , building
								//calScore_list[14] = (hsv_score + (double) 1.0) / 2.0 + (fea_score_4_MD * (double) 2.0);

								//calScore_list[15] = (hsv_score_5_MOA + (double) 1.0) / 2.0 + (fea_score * (double) 2.0);
								//calScore_list[16] = (hsv_score + (double) 1.0) / 2.0 + (fea_score_5_OA * (double) 2.0);
								//calScore_list[17] = (hsv_score + (double) 1.0) / 2.0 + (fea_score_6_MOA * (double) 2.0);

								//calScore_list[18] = (hsv_score + (double) 1.0) / 2.0 + (fea_score_5_OA * 0.75 + fea_score_6_MOA *0.25);
								//calScore_list[19] = (hsv_score + (double) 1.0) / 2.0 + (fea_score_5_OA * 0.5 + fea_score_6_MOA *0.5);

								//calScore_list[20] = (hsv_score_4_D + (double) 1.0) / 2.0 + (fea_score_5_OA * 0.75 + fea_score_6_MOA *0.25);
								//calScore_list[21] = (hsv_score_4_D + (double) 1.0) / 2.0 + (fea_score_5_OA * 0.5 + fea_score_6_MOA *0.5);

								//calScore_list[22] = (hsv_score_5_MOA + (double) 1.0) / 2.0 + (fea_score_5_OA * 0.75 + fea_score_6_MOA *0.25);
								//calScore_list[23] = (hsv_score_5_MOA + (double) 1.0) / 2.0 + (fea_score_5_OA * 0.5 + fea_score_6_MOA *0.5);

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
				// ===== ===== ===== ===== ===== ===== ===== ===== =====
				// ===== ===== ===== @Sort ===== ===== ===== ===== =====
				// ===== ===== ===== ===== ===== ===== ===== ===== =====
				// ===== sort method 1 // for hsv? , or (1 - fea)
				for each (int i in valid_indexs) {
					for (int k = 0; k < iss_list_[i].size(); k++) {
						sort(iss_list_[i][k].rbegin(), iss_list_[i][k].rend());

						//@save
						//vector<ImgScore> save = iss_list_[i][k];
						//save_D_C_R(save, i, k);

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

				for (int k = 0; k < iss_list_[0].size(); k++) {
					vector<ScoreReport> for_best_threshold;

					for each (int i in valid_indexs) {
						for_best_threshold.push_back(bestMData_[i].srs[k]);

						if (k == 5)
							bestMData_[i].srs[k].reportSorted(200);
					}
					m_result.add_acc(for_best_threshold);
					m_result.add_best_threshold(for_best_threshold);

				} //each method

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
