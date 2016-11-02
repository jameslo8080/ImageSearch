#include "feature_extract.h"

Mat rgbMat_to_hsvHist(Mat src_base) {
	Mat hsv_base;
	cvtColor(src_base, hsv_base, COLOR_BGR2HSV);

	int h_bins = 16, s_bins = 20, v_bins = 3;
	int histSize[] = { h_bins, s_bins, v_bins };

	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	float v_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges, v_ranges };

	int channels[] = { 0, 1, 2 };

	MatND hist_base;

	calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, true);
	// normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	return hist_base;
}


vector<Mat> divide_image(Mat raw_img, int n_row, int n_col) {
	int w = raw_img.cols, h = raw_img.rows;
	int iw = w / n_col, ih = h / n_row;
	vector<Mat> res;

	for (int i = 0; i < n_col; ++i)
		for (int j = 0; j < n_row; ++j){
			res.push_back(Mat(raw_img, Rect(i*iw, j*ih, iw, ih))); // add .clone() wull be deep copy
		}
	return res;
}

vector<Mat> rgbMat_to_divided_hsvHist(Mat src_base, int n_row, int n_col) {
	vector<Mat> imgs = divide_image(src_base, n_row, n_col);
	vector<Mat> res;
	for (auto img : imgs)
		res.push_back(rgbMat_to_hsvHist(img));
	return res;
}


Mat calSURFDescriptor(Mat input) {
	SurfFeatureDetector detector;
	SurfDescriptorExtractor extractor;
	vector<KeyPoint> keyPoints;
	Mat descriptors;
	detector.detect(input, keyPoints);
	extractor.compute(input, keyPoints, descriptors);
	return descriptors;
}

Mat calSIFTDescriptor(Mat input) {
	SiftFeatureDetector  sift;
	vector<KeyPoint> keyPoints;
	Mat descriptors;
	sift.detect(input, keyPoints);
	sift.compute(input, keyPoints, descriptors);
	return descriptors;
}

Mat calORBDescriptor(Mat input) {
	OrbFeatureDetector detector;
	OrbDescriptorExtractor extractor;
	vector<KeyPoint> keyPoints;
	Mat descriptors;
	detector.detect(input, keyPoints);
	extractor.compute(input, keyPoints, descriptors);
	return descriptors;
}
