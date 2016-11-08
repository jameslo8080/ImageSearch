#include "feature_extract.h"

Mat equalized(Mat src) {
	Mat dst;
	cvtColor(src, src, CV_BGR2GRAY);

	equalizeHist(src, dst);
	return dst;
}

Mat greyscaleHist(Mat src_base) {
	int bins = 24;
	int histSize[] = { bins };

	float lranges[] = { 0, 256 };
	const float* ranges[] = { lranges };
	
	Mat hist;
	int channels[] = { 0 };

	calcHist(&src_base, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);

	return hist;
}

Mat rgbHist(Mat src_base) {
	float range[] = { 0, 256 };

	int b_bins = 8, g_bins = 8, r_bins = 8;
	int histSize[] = { b_bins, g_bins, r_bins };

	const float* ranges[] = { range, range, range };
	int channels[] = { 0, 1, 2 };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;
	MatND hist_base;
	calcHist(&src_base, 1, channels, Mat(), b_hist, 2, histSize, ranges, uniform, accumulate);
	return hist_base;
}

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

Mat contrast_brightness_change(const Mat& img, double alpha, double beta) {
	Mat new_image = Mat::zeros(img.size(), img.type());

	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			for (int c = 0; c < img.channels(); c++) {
				new_image.at<Vec3b>(y, x)[c] =
					saturate_cast<uchar>(alpha*(img.at<Vec3b>(y, x)[c])) + beta;
			}
		}
	}
	return new_image;
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

Mat crop_ellipse(const Mat& im1) {
	double cx = im1.cols / 2.0, cy = im1.rows / 2.0;
	double lx = cx * 0.8, ly = cy*0.8;

	Mat im2(im1.rows, im1.cols, CV_8UC1, Scalar(0, 0, 0));
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
