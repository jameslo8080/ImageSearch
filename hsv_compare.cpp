#include "compare.h"

void hsv_compare(Mat src_input, int index) {

	//// compare hsv
	Mat src_hsv = rgbMat_to_hsvHist(src_input);
	vector<ImgScore> res0;
	vector<Mat> features = load_features();
	for (int i = 0; i < features.size(); ++i) res0.push_back(ImgScore(i, compareHist(features[i], src_hsv, 0)));
	sort(res0.rbegin(), res0.rend());
	res0.resize(100);
	printf("res0 Acc: %lf \n", validate_fit(res0, index));
	printf("Done \n");
}
