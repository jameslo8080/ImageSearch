#include "compare.h"

// pixel + hsv
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
		imgScoreResult[i] = ImgScore(i, score);
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}

// equalized + greyscale
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
		imgScoreResult[i] = ImgScore(i, score);
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}

// hsv + equalized + greyscale
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
		imgScoreResult[i] = ImgScore(i, score);
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}

// PSNR + equalized + greyscale
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
		imgScoreResult[i] = ImgScore(i, score);
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}

// contrast + hsv hist
double combin_compare_5(Mat src_input, int inputIndex, double contrast) {
	Mat im1 = contrast_brightness_change(src_input, contrast, 0);
	Mat hsv_input = rgbMat_to_hsvHist(im1);

	vector<Mat> imgs = load_imgs();

	int len = imgs.size();

	vector<ImgScore> imgScoreResult(len);
	for (int i = 0; i < len; ++i) {
		Mat im2 = contrast_brightness_change(imgs[i], contrast, 0);
		Mat hsvhist = rgbMat_to_hsvHist(im2);
		double score = hist_cmp(hsvhist, hsv_input);
		imgScoreResult[i] = ImgScore(i, score);
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}

// contrast + split hsv
double combin_compare_6(Mat src_input, int inputIndex, double contrast) {
	Mat im1 = contrast_brightness_change(src_input, contrast, 0);
	vector<Mat> hsv_input = rgbMat_to_divided_hsvHist(im1, 3, 3);

	vector<Mat> imgs = load_imgs();

	int len = imgs.size();

	vector<ImgScore> imgScoreResult(len);
	for (int i = 0; i < len; ++i) {
		Mat im2 = contrast_brightness_change(imgs[i], contrast, 0);
		double score = hsv_splited_cmp(hsv_input, im2, 3, 3, 0);
		imgScoreResult[i] = ImgScore(i, score);
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}


// PSNR + contrast
double combin_compare_7(Mat src_input, int inputIndex, double contrast) {
	Mat im1 = contrast_brightness_change(src_input, contrast, 0);
	Mat hsv_input = rgbMat_to_hsvHist(im1);

	vector<Mat> imgs = load_imgs();

	int len = imgs.size();

	vector<ImgScore> imgScoreResult(len);
	for (int i = 0; i < len; ++i) {
		Mat im2 = contrast_brightness_change(imgs[i], contrast, 0);
		Mat img = rgbMat_to_hsvHist(im2);

		imgScoreResult[i] = ImgScore(i, getPSNR(hsv_input, img));
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}

// PSNR + divided + contrast
double combin_compare_7a(Mat src_input, int inputIndex, double contrast) {
	Mat im1 = contrast_brightness_change(src_input, contrast, 0);
	vector<Mat> hsv_input = rgbMat_to_divided_hsvHist(im1, 3, 3);

	vector<Mat> imgs = load_imgs();

	int len = imgs.size();

	vector<ImgScore> imgScoreResult(len);
	for (int i = 0; i < len; ++i) {
		Mat im2 = contrast_brightness_change(imgs[i], contrast, 0);
		vector<Mat> img = rgbMat_to_divided_hsvHist(im2, 3, 3);

		imgScoreResult[i] = ImgScore(i, getPSNR_divided(hsv_input, img, 3, 3));
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}

// MSSIM + contrast
double combin_compare_8(Mat src_input, int inputIndex, double contrast) {
	Mat im1 = contrast_brightness_change(src_input, contrast, 0);
	Mat hsv_input = rgbMat_to_hsvHist(im1);

	vector<Mat> imgs = load_imgs();

	int len = imgs.size();

	vector<ImgScore> imgScoreResult(len);
	for (int i = 0; i < len; ++i) {
		Mat im2 = contrast_brightness_change(imgs[i], contrast, 0);
		Mat img = rgbMat_to_hsvHist(im2);
		Scalar ret = getMSSIM(img, hsv_input);
		imgScoreResult[i] = ImgScore(i, ret[0]);
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}

// splited + equalized + greyscale

// rgb + splited

// rgb + circle mask

// hsv + circle mask

// equalized + greyscale + circle mask

// hsv + surf

// hsv + sift

// hsv + orb

// canny

// glcm

// lbp
