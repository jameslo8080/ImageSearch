#include "compare.h"

double hsv_compare(Mat src_input, int index) {

	//// compare hsv
	Mat src_hsv = rgbMat_to_hsvHist(src_input);
	vector<ImgScore> res;
	vector<Mat> features = load_features();
	for (int i = 0; i < features.size(); ++i) res.push_back(ImgScore(i, compareHist(features[i], src_hsv, 0)));
	sort(res.rbegin(), res.rend());
	res.resize(100);


	double acc = validate_fit(res, index);
	printf("res Acc: %lf \n", acc);
	printf("Done \n");
	return acc;
}

double hsv_split_compare(Mat src_input, int index) {
	int nrow = 5, rcol = 5;
	int tot = nrow*rcol;
	vector<Mat> src_hsv = rgbMat_to_divided_hsvHist(src_input, nrow, rcol);

	vector<ImgScore> res;
	vector<Mat> imgs = load_imgs();
	for (int i = 0; i < imgs.size(); ++i) {
		vector<Mat> splited_img = rgbMat_to_divided_hsvHist(imgs[i], nrow, rcol);
		double scores = 0;
		double sums = 0;
		for (int i = 0; i < tot; ++i)
			sums += compareHist(splited_img[i], src_hsv[i], 0);
		res.push_back(ImgScore(i, sums / (double)(tot)));
	}
	sort(res.rbegin(), res.rend());
	res.resize(100);
	//res.erase(remove_if(res.begin(), res.end(), [](const ImgScore &x){return x.score < 0.3; }), res.end());

	double acc = validate_fit(res, index);
	printf("res Acc: %lf \n", acc);
	printf("Done \n");
	return acc;
}