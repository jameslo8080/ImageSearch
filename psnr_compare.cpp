#include "compare.h"

double psnr_compare(Mat src_input, int inputIndex) {

	vector<Mat> features = load_hsvHist();
	Mat src_hsv = rgbMat_to_hsvHist(src_input);
	vector<ImgScore> imgScoreResult;
	for (int i = 0; i < features.size(); ++i) imgScoreResult.push_back(ImgScore(i, getPSNR(features[i], src_hsv)));

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}
