#include "feature_extract.h"

Mat rgbMat_to_hsvHist(Mat src_base)
{
	Mat hsv_base;
	/// Convert to HSV
	cvtColor(src_base, hsv_base, COLOR_BGR2HSV);

	/// Using 50 bins for hue and 60 for saturation
	int h_bins = 16; int s_bins = 20; int v_bins = 3;
	int histSize[] = { h_bins, s_bins, v_bins };

	// hue varies from 0 to 179, saturation from 0 to 255
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	float v_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges, v_ranges };

	// Use the o-th and 1-st channels
	int channels[] = { 0, 1, 2 };

	/// Histograms
	MatND hist_base;

	/// Calculate the histograms for the HSV images
	calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, true);
	// normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	return hist_base;
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