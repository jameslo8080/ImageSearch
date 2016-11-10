#include "compare.h"

double pixel_compare(Mat src_input, int index) {

	vector<Mat> imgs = load_imgs();

	vector<ImgScore> imgScoreResult;
	for (int i = 0; i < imgs.size(); ++i) imgScoreResult.push_back(ImgScore(i, pixel_cmp(imgs[i], src_input)));

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, index);
	sr.report();

	return sr.acc100;
}
