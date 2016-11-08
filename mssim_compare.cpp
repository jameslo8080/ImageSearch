#include "compare.h"


double mssim_compare(Mat src_input, int inputIndex) {

	vector<Mat> features = load_hsvHist();
	Mat src_hsv = rgbMat_to_hsvHist(src_input);
	vector<ImgScore> imgScoreResult;
	for (int i = 0; i < features.size(); ++i) {
		Scalar res = getMSSIM(features[i], src_hsv);
		double score = res[0];
		imgScoreResult.push_back(ImgScore(i, score));
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}