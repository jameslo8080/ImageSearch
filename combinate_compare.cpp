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
				pixelscore[i] = pixel_cmp(src_input, imgs[i]);
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
double combin_compare_6(Mat src_input, int inputIndex, double contrast, double bright) {
		Mat im1 = contrast_brightness_change(src_input, contrast, bright);
		vector<Mat> hsv_input = rgbMat_to_divided_hsvHist(im1, 3, 3);

		vector<Mat> imgs = load_imgs();

		int len = imgs.size();

		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat im2 = contrast_brightness_change(imgs[i], contrast, bright);
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
		Mat im1 = contrast_brightness_change(src_input, contrast, 20);
		Mat hsv_input = rgbMat_to_hsvHist(im1);

		vector<Mat> imgs = load_imgs();

		int len = imgs.size();

		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat im2 = contrast_brightness_change(imgs[i], contrast, 20);
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
double combin_compare_9(Mat src_input, int inputIndex) {
		vector<Mat> imgs = load_imgs();

		Mat greyMat = equalized(src_input);
		vector<Mat> in_im = divide_image(greyMat, 3, 3);
		vector<Mat> greyHist = greyscaleHist(in_im);


		int len = imgs.size();
		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat greyMat2 = equalized(imgs[i]);

				vector<Mat> t_im = divide_image(greyMat2, 3, 3);
				vector<Mat> greyHist2 = greyscaleHist(t_im);

				double score = hsv_splited_cmp(greyHist, greyHist2, 3, 3, 0);

				imgScoreResult[i] = ImgScore(i, score);
		}

		sort(imgScoreResult.rbegin(), imgScoreResult.rend());
		ScoreReport sr(imgScoreResult, inputIndex);
		sr.report();

		return sr.acc100;
}

// contrast rgb + splited
double combin_compare_10(Mat src_input, int inputIndex, double contrast) {
		vector<Mat> imgs = load_imgs();
		int len = imgs.size();

		Mat tim1 = contrast_brightness_change(src_input, contrast, 0);
		vector<Mat> im1 = divide_image(tim1, 3, 3);
		vector<Mat> im1_hist = rgbHist(im1);

		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat tim2 = contrast_brightness_change(imgs[i], contrast, 0);
				vector<Mat> im2 = divide_image(tim2, 3, 3);
				vector<Mat> im2_hist = rgbHist(im2);

				double score = hsv_splited_cmp(im1_hist, im2_hist, 3, 3, 0);
				imgScoreResult[i] = ImgScore(i, score);
		}

		sort(imgScoreResult.rbegin(), imgScoreResult.rend());
		ScoreReport sr(imgScoreResult, inputIndex);
		sr.report();

		return sr.acc100;
}

// contrast + rgb + circle mask
double combin_compare_11(Mat src_input, int inputIndex, double contrast) {
		vector<Mat> imgs = load_imgs();
		int len = imgs.size();

		Mat tim1 = contrast_brightness_change(src_input, contrast, 0);
		Mat im1 = crop_ellipse(tim1);
		Mat im1_hist = rgbHist(im1);

		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat tim2 = contrast_brightness_change(imgs[i], contrast, 0);
				Mat im2 = crop_ellipse(tim2);
				Mat im2_hist = rgbHist(im2);

				double score = hist_cmp(im1_hist, im2_hist, 0);
				imgScoreResult[i] = ImgScore(i, score);
		}

		sort(imgScoreResult.rbegin(), imgScoreResult.rend());
		ScoreReport sr(imgScoreResult, inputIndex);
		sr.report();

		return sr.acc100;
}

// contrast + hsv + circle mask
double combin_compare_12(Mat src_input, int inputIndex, double contrast) {
		vector<Mat> imgs = load_imgs();
		int len = imgs.size();

		Mat tim1 = contrast_brightness_change(src_input, contrast, 0);
		Mat im1 = crop_ellipse(tim1);
		Mat im1_hist = rgbMat_to_hsvHist(im1);

		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat tim2 = contrast_brightness_change(imgs[i], contrast, 0);
				Mat im2 = crop_ellipse(tim2);
				Mat im2_hist = rgbMat_to_hsvHist(im2);

				double score = hist_cmp(im1_hist, im2_hist, 0);
				imgScoreResult[i] = ImgScore(i, score);
		}

		sort(imgScoreResult.rbegin(), imgScoreResult.rend());
		ScoreReport sr(imgScoreResult, inputIndex);
		sr.report();

		return sr.acc100;
}

// equalized + greyscale + circle mask
double combin_compare_13(Mat src_input, int inputIndex) {
		vector<Mat> imgs = load_imgs();
		int len = imgs.size();

		Mat im1 = crop_ellipse(src_input);
		Mat greyMat = equalized(im1);
		Mat greyHist = greyscaleHist(greyMat);


		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat im2 = crop_ellipse(imgs[i]);
				Mat greyMat2 = equalized(im2);
				Mat greyHist2 = greyscaleHist(greyMat2);


				double score = hist_cmp(greyHist, greyHist2, 0);
				imgScoreResult[i] = ImgScore(i, score);
		}

		sort(imgScoreResult.rbegin(), imgScoreResult.rend());
		ScoreReport sr(imgScoreResult, inputIndex);
		sr.report();

		return sr.acc100;
}

// hsv + circle mask + PSNR
double combin_compare_14(Mat src_input, int inputIndex, double contrast) {
		vector<Mat> imgs = load_imgs();
		int len = imgs.size();

		Mat tim1 = contrast_brightness_change(src_input, contrast, 0);
		Mat im1 = crop_ellipse(tim1);
		Mat im1_hist = rgbMat_to_hsvHist(im1);

		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat tim2 = contrast_brightness_change(imgs[i], contrast, 0);
				Mat im2 = crop_ellipse(tim2);
				Mat im2_hist = rgbMat_to_hsvHist(im2);

				double score = getPSNR(im1_hist, im2_hist);
				imgScoreResult[i] = ImgScore(i, score);
		}

		sort(imgScoreResult.rbegin(), imgScoreResult.rend());
		ScoreReport sr(imgScoreResult, inputIndex);
		sr.report();

		return sr.acc100;
}

// contrast + hsv + surf
double combin_compare_15(Mat src_input, int inputIndex, double contrast) {
		Mat im1 = contrast_brightness_change(src_input, contrast, 0);
		Mat des1 = calSURFDescriptor(im1);

		vector<Mat> imgs = load_imgs();

		int len = imgs.size();

		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat im2 = contrast_brightness_change(imgs[i], contrast, 0);
				Mat des2 = calSURFDescriptor(im2);

				double score = feature_cmp(des1, des2);
				imgScoreResult[i] = ImgScore(i, score);
		}

		sort(imgScoreResult.begin(), imgScoreResult.end());
		ScoreReport sr(imgScoreResult, inputIndex);
		sr.report();

		return sr.acc100;
}

// contrast + hsv + sift
double combin_compare_16(Mat src_input, int inputIndex, double contrast) {
		Mat im1 = contrast_brightness_change(src_input, contrast, 0);
		Mat des1 = calSIFTDescriptor(im1);

		vector<Mat> imgs = load_imgs();

		int len = imgs.size();

		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat im2 = contrast_brightness_change(imgs[i], contrast, 0);
				Mat des2 = calSIFTDescriptor(im2);

				double score = feature_cmp(des1, des2);
				imgScoreResult[i] = ImgScore(i, score);
		}

		sort(imgScoreResult.begin(), imgScoreResult.end());
		ScoreReport sr(imgScoreResult, inputIndex);
		sr.report();

		return sr.acc100;
}

// contrast + hsv + orb
double combin_compare_17(Mat src_input, int inputIndex, double contrast) {
		Mat im1 = contrast_brightness_change(src_input, contrast, 0);
		Mat des1 = calORBDescriptor(im1);

		vector<Mat> imgs = load_imgs();

		int len = imgs.size();

		vector<ImgScore> imgScoreResult(len);
		for (int i = 0; i < len; ++i) {
				Mat im2 = contrast_brightness_change(imgs[i], contrast, 0);
				Mat des2 = calORBDescriptor(im2);

				double score = feature_cmp(des1, des2);
				imgScoreResult[i] = ImgScore(i, score);
		}

		sort(imgScoreResult.begin(), imgScoreResult.end());
		ScoreReport sr(imgScoreResult, inputIndex);
		sr.report();

		return sr.acc100;
}

// canny
double combin_compare_18(Mat src_input, int inputIndex, double contrast) {
	vector<Mat> imgs = load_imgs();
	int len = imgs.size();

	Mat tim1 = contrast_brightness_change(src_input, contrast, 0);
	vector<vector<Point> > imc_cont1 = getCannyContours(tim1);

	vector<ImgScore> imgScoreResult(len);
	for (int i = 0; i < len; ++i) {

		Mat tim2 = contrast_brightness_change(imgs[i], contrast, 0);
		vector<vector<Point> > imc_cont2 = getCannyContours(tim2);

		double score = contour_cmp(imc_cont1, imc_cont2);
		
		imgScoreResult[i] = ImgScore(i, score);
	}

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}
