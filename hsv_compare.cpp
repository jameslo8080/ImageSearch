#include "compare.h"

double hsv_compare(Mat src_input, int inputIndex) {
	vector<Mat> features = load_features();
	Mat src_hsv = rgbMat_to_hsvHist(src_input);
	vector<ImgScore> imgScoreResult;
	for (int i = 0; i < features.size(); ++i) imgScoreResult.push_back(ImgScore(i, compareHist(features[i], src_hsv, 0)));

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return 0;
}

double hsv_split_compare(Mat src_input, int inputIndex) {
	return  hsv_split_compare(src_input, inputIndex, true, 2, 2);
}

double hsv_split_compare(Mat src_input, int inputIndex, bool output, int row, int col) {
	vector<Mat> imgs = load_imgs();

	int tot = row*col;
	vector<Mat> src_hsv = rgbMat_to_divided_hsvHist(src_input, row, col);

	vector<ImgScore> imgScoreResult;

	for (int i = 0; i < imgs.size(); ++i) {
		vector<Mat> splited_img = rgbMat_to_divided_hsvHist(imgs[i], row, col);
		double scores = 0;
		double sums = 0;
		for (int i = 0; i < tot; ++i)
			sums += fabs(compareHist(splited_img[i], src_hsv[i], 0));
		imgScoreResult.push_back(ImgScore(i, sums / (double)(tot)));
	}

	// sort
	sort(imgScoreResult.rbegin(), imgScoreResult.rend());

	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report(row, col);

	return 0;
}
