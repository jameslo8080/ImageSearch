#include "core_cmp.h"

double pixel_compare(const Mat& img1, const Mat& img2) {
	int w = img1.cols, h = img2.rows;
	Mat new_img2;
	resize(img2, new_img2, img1.size());
	double sum = 0;
	for (int i = 0; i < w; i++)for (int j = 0; j < h; j++)
	{
		sum += abs(img1.at<uchar>(j, i) - new_img2.at<uchar>(j, i));
	}
	return sum;
}
