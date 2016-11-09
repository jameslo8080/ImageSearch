#include "feature_extract.h"

Mat equalized(Mat src) {
	Mat dst;
	cvtColor(src, src, CV_BGR2GRAY);

	equalizeHist(src, dst);
	return dst;
}

Mat greyscaleHist(const Mat& src_base) {
	int bins = 24;
	int histSize[] = { bins };

	float lranges[] = { 0, 256 };
	const float* ranges[] = { lranges };
	
	Mat hist;
	int channels[] = { 0 };

	calcHist(&src_base, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);

	return hist;
}

vector<Mat> greyscaleHist(const vector<Mat>& imgs) {
	vector<Mat> res;
	for (auto img : imgs) res.push_back(greyscaleHist(img));
	return res;
}

Mat rgbHist(const Mat& src_base) {
	float range[] = { 0, 256 };

	int b_bins = 8, g_bins = 8, r_bins = 8;
	int histSize[] = { b_bins, g_bins, r_bins };

	const float* ranges[] = { range, range, range };
	int channels[] = { 0, 1, 2 };

	MatND hist_base;
	calcHist(&src_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, true);
	return hist_base;
}


vector<Mat> rgbHist(const vector<Mat>& src_base) {
	vector<Mat> res;
	for (auto img : src_base) res.push_back(rgbHist(img));
	return res;
}

Mat rgbMat_to_hsvHist(Mat src_base) {
	Mat hsv_base;
	cvtColor(src_base, hsv_base, COLOR_BGR2HSV);

	int h_bins = 8, s_bins = 8;
	int histSize[] = { h_bins, s_bins};

	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges};

	int channels[] = { 0, 1};

	MatND hist_base;

	calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, true);

	return hist_base;
}

Mat contrast_brightness_change(const Mat& img, double alpha, double beta) {
	Mat new_image = Mat::zeros(img.size(), img.type());

	for (int y = 0; y < img.rows; y++)
		for (int x = 0; x < img.cols; x++)
			for (int c = 0; c < img.channels(); c++)
				new_image.at<Vec3b>(y, x)[c] =
					saturate_cast<uchar>(alpha*(img.at<Vec3b>(y, x)[c])) + beta;

	return new_image;
}

vector<Mat> divide_image(Mat raw_img, int n_row, int n_col) {
	int w = raw_img.cols, h = raw_img.rows;
	int iw = w / n_col, ih = h / n_row;
	vector<Mat> res;

	for (int i = 0; i < n_col; ++i)
		for (int j = 0; j < n_row; ++j)
			res.push_back(Mat(raw_img, Rect(i*iw, j*ih, iw, ih))); // add .clone() wull be deep copy

	return res;
}

vector<Mat> rgbMat_to_divided_hsvHist(Mat src_base, int n_row, int n_col) {
	vector<Mat> res;

	vector<Mat> imgs = divide_image(src_base, n_row, n_col);
	for (auto img : imgs) res.push_back(rgbMat_to_hsvHist(img));
	return res;
}

Mat crop_ellipse(const Mat& im1) {
	double cx = im1.cols / 2.0, cy = im1.rows / 2.0;
	double lx = cx * 0.8, ly = cy*0.8;

	Mat im2(im1.rows, im1.cols, im1.type(), Scalar(0, 0, 0));
	ellipse(im2, Point(cx, cy), Size(lx, ly), 0, 0, 360, Scalar(255, 255, 255), -1, 8);
	Mat res;
	bitwise_and(im1, im2, res);
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

// =====================

Mat surf_cal_descriptor_bow(BOWImgDescriptorExtractor& bowExtractor, const Mat &db_img) {
	SurfFeatureDetector detector;
	vector<KeyPoint> keyPoints;
	Mat descriptors;
	detector.detect(db_img, keyPoints);
	bowExtractor.compute(db_img, keyPoints, descriptors);
	return descriptors;
}

Mat sift_cal_descriptor_bow(BOWImgDescriptorExtractor& bowExtractor, const Mat &db_img) {
	SiftFeatureDetector detector;
	vector<KeyPoint> keyPoints;
	Mat descriptors;
	detector.detect(db_img, keyPoints);
	bowExtractor.compute(db_img, keyPoints, descriptors);
	return descriptors;
}

Mat orb_cal_descriptor_bow(BOWImgDescriptorExtractor& bowExtractor, const Mat &db_img) {
	OrbFeatureDetector detector;
	vector<KeyPoint> keyPoints;
	Mat descriptors;
	detector.detect(db_img, keyPoints);
	bowExtractor.compute(db_img, keyPoints, descriptors);
	return descriptors;
}

Mat cal_descriptor_bow(BOWImgDescriptorExtractor& bowExtractor, const Mat &db_img, string type) {
	if (type == "SURF")
		return surf_cal_descriptor_bow(bowExtractor, db_img);
	else if (type == "SIFT")
		return sift_cal_descriptor_bow(bowExtractor, db_img);
	else
		return orb_cal_descriptor_bow(bowExtractor, db_img);

}

vector<vector<Point> > getCannyContours(const Mat& img, double thresh) {
	Mat src_gray;
	cvtColor(img, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));

	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Canny(src_gray, canny_output, thresh, thresh * 2, 3);
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	return contours;
}