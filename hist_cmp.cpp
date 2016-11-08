#include "core_cmp.h"

double hist_cmp(const Mat& img1, const Mat& img2, int method) {
	return compareHist(img1, img2, 0);
}

/**
asdasd
*/
double hsv_cmp(const Mat& img1, const Mat& img2, int method) {
	Mat himg1 = rgbMat_to_hsvHist(img1);
	Mat himg2 = rgbMat_to_hsvHist(img2);
	return compareHist(himg1, himg2, method);
}

double hsv_splited_cmp(const Mat& img1, const Mat& img2, int row, int col, int method) {
	int tot = row*col;
	vector<Mat> splited_img1 = rgbMat_to_divided_hsvHist(img1, row, col);
	vector<Mat> splited_img2 = rgbMat_to_divided_hsvHist(img2, row, col);
	double scores = 0;
	for (int i = 0; i < tot; ++i)
		scores += fabs(compareHist(splited_img1[i], splited_img2[i], method));
	return scores / (double)tot;
}

double hsv_splited_cmp(const vector<Mat>& splited_img1, const Mat& img2, int row, int col, int method) {
	int tot = row*col;
	vector<Mat> splited_img2 = rgbMat_to_divided_hsvHist(img2, row, col);
	double scores = 0;
	for (int i = 0; i < tot; ++i)
		scores += fabs(compareHist(splited_img1[i], splited_img2[i], method));
	return scores / (double)tot;
}

double hsv_splited_cmp(const vector<Mat>& splited_img1, const vector<Mat>& splited_img2, int row, int col, int method) {
	int tot = row*col;
	double scores = 0;
	for (int i = 0; i < tot; ++i)
		scores += fabs(compareHist(splited_img1[i], splited_img2[i], method));
	return scores / (double)tot;
}
