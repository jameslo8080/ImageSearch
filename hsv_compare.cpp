#include "compare.h"

double hsv_compare(Mat src_input, int inputIndex) {
	vector<Mat> features = load_hsvHist();
	Mat src_hsv = rgbMat_to_hsvHist(src_input);
	vector<ImgScore> imgScoreResult;
	for (int i = 0; i < features.size(); ++i) imgScoreResult.push_back(ImgScore(i, hist_cmp(features[i], src_hsv)));

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}

double hsv_split_compare(Mat src_input, int inputIndex) {
	return  hsv_split_compare(src_input, inputIndex, true, 2, 2);
}

double hsv_split_compare(Mat src_input, int inputIndex, bool output, int row, int col) {
	vector<Mat> imgs = load_imgs();
	vector<Mat> src_hsv = rgbMat_to_divided_hsvHist(src_input, row, col);
	vector<ImgScore> imgScoreResult;

	for (int i = 0; i < imgs.size(); ++i) {
		vector<Mat> splited_img = rgbMat_to_divided_hsvHist(imgs[i], row, col);
		double scores = hsv_splited_cmp(src_hsv, imgs[i], row, col);
		imgScoreResult.push_back(ImgScore(i, scores));
	}

	// sort
	sort(imgScoreResult.rbegin(), imgScoreResult.rend());

	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report(row, col);

	return sr.acc100;
}
