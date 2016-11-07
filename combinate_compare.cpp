#include "compare.h"

double combin_compare_1(Mat src_input, int inputIndex) {
	Mat hsv_input = rgbMat_to_hsvHist(src_input);
	vector<Mat> imgs = load_imgs();
	vector<Mat> hsvHist = load_hsvHist();
	int len = imgs.size();


	vector<double> pixelscore(len);
	vector<double> hsvhistscore(len);
	for (int i = 0; i < len; ++i) {
		pixelscore[i] = pixel_compare(src_input, imgs[i]);
		hsvhistscore[i] = hist_cmp(hsv_input, hsvHist[i]);
	}

	vector<double> n_pixelscore = normalize_vec(pixelscore);
	vector<double> n_hsvhistscore = normalize_vec(hsvhistscore);
	vector<ImgScore> imgScoreResult(len);
	for (int i = 0; i < len; ++i) {
		double score = 0.25*n_pixelscore[i] + 0.75*n_hsvhistscore[i];
		imgScoreResult.push_back(ImgScore(i, score));
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}

double combin_compare_2(Mat src_input, int inputIndex) {
	
	Mat greyMat = equalized(src_input);
	Mat greyHist = greyscaleHist(greyMat);

	vector<Mat> imgs = load_imgs();
	int len = imgs.size();

	vector<ImgScore> imgScoreResult(len);
	for (int i = 0; i < len; ++i) {

		Mat greyMat2 = equalized(imgs[i]);
		Mat greyHist2 = greyscaleHist(greyMat2);

		double score = hist_cmp(greyHist, greyHist2);
		imgScoreResult.push_back(ImgScore(i, score));
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}



double combin_compare_3(Mat src_input, int inputIndex, double r1, double r2) {
	Mat hsv_input = rgbMat_to_hsvHist(src_input);
	vector<Mat> imgs = load_imgs();
	vector<Mat> hsvHist = load_hsvHist();

	Mat greyMat = equalized(src_input);
	Mat greyHist = greyscaleHist(greyMat);

	int len = imgs.size();

	vector<double> hsvhistscore(len);
	vector<double> greyscalescore(len);
	for (int i = 0; i < len; ++i) {
		Mat greyMat2 = equalized(imgs[i]);
		Mat greyHist2 = greyscaleHist(greyMat2);

		greyscalescore[i] = hist_cmp(greyHist, greyHist2);
		hsvhistscore[i] = hist_cmp(hsv_input, hsvHist[i]);
	}


	vector<double> n_hsvhistscore = normalize_vec(hsvhistscore);
	vector<double> n_greyscalescore = normalize_vec(greyscalescore);
	vector<ImgScore> imgScoreResult(len);
	for (int i = 0; i < len; ++i) {
		double score = r1*n_hsvhistscore[i] + r2*n_greyscalescore[i];
		imgScoreResult.push_back(ImgScore(i, score));
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}


double combin_compare_4(Mat src_input, int inputIndex, double r1, double r2) {
	vector<Mat> imgs = load_imgs();
	vector<Mat> hsvHist = load_hsvHist();

	Mat greyMat = equalized(src_input);
	Mat greyHist = greyscaleHist(greyMat);

	int len = imgs.size();

	vector<double> psnrscore(len);
	vector<double> greyscalescore(len);
	for (int i = 0; i < len; ++i) {
		Mat greyMat2 = equalized(imgs[i]);
		Mat greyHist2 = greyscaleHist(greyMat2);

		greyscalescore[i] = hist_cmp(greyHist, greyHist2);
		psnrscore[i] = getPSNR(greyHist, greyHist2);
	}


	vector<double> n_psnrscore = normalize_vec(psnrscore);
	vector<double> n_greyscalescore = normalize_vec(greyscalescore);
	vector<ImgScore> imgScoreResult(len);
	for (int i = 0; i < len; ++i) {
		double score = r1*psnrscore[i] + r2*greyscalescore[i];
		imgScoreResult.push_back(ImgScore(i, score));
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}
