#include "compare.h"

double combin_compare_1(Mat src_input, int inputIndex) {
	vector<Mat> imgs = load_imgs();
	vector<Mat> hsvHist = load_hsvHist();

	Mat hsv_input = rgbMat_to_hsvHist(src_input);

	int len = imgs.size();

	
	return 0;
}
