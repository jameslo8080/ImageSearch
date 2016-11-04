#include "utils.h"


int get_group(int db_id) {
	return (db_id < 0 || db_id > 999) ? -1 : db_id / 100;
}

Mat convert_2DMat_to_1DMat(Mat inMat) {
	Mat outMat(1, inMat.rows*inMat.cols, CV_32F);

	int ii = 0;
	for (int i = 0; i < inMat.rows; i++) {
		for (int j = 0; j < inMat.cols; j++) {
			outMat.at<float>(0, ii++) = inMat.at<uchar>(i, j);
		}
	}

	return outMat;
}