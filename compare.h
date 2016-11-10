#pragma once 

#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <map>
#include <algorithm>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "feature_loader.h"
#include "feature_extract.h"
#include "feature_preprocess.h"
#include "ScoreReport.h"
#include "ml.h"
#include "utils.h"
#include "core_cmp.h"

using namespace std;
using namespace cv;

#define METHOD_COUNT 24

double pixel_compare(Mat src_input, int index);
double hsv_compare(Mat src_input, int index);
double hsv_split_compare(Mat src_input, int index);
double hsv_split_compare(Mat src_input, int index, bool output, int row, int col);
double svm_compare(Mat src_input, int index);
double surf_compare(Mat src_input, int index);
double sift_compare(Mat src_input, int index);
double orb_compare(Mat src_input, int index);
double psnr_compare(Mat src_input, int inputIndex);
double mssim_compare(Mat src_input, int inputIndex);


void double_compare();
void on_double_compare();
void double_compare_bus();

vector<int> new_compare(Mat src_color, int inputIndex, double the);

double combin_compare_1(Mat src_input, int inputIndex);
double combin_compare_2(Mat src_input, int inputIndex);
double combin_compare_3(Mat src_input, int inputIndex, double r1 = 0.5, double r2 = 0.5);
double combin_compare_4(Mat src_input, int inputIndex, double r1 = 0.5, double r2 = 0.5);
double combin_compare_5(Mat src_input, int inputIndex, double contrast = 1.2);
double combin_compare_6(Mat src_input, int inputIndex, double contrast = 1.2, double bright = 0);
double combin_compare_7(Mat src_input, int inputIndex, double contrast = 1.2);
double combin_compare_7a(Mat src_input, int inputIndex, double contrast = 1.2);
double combin_compare_8(Mat src_input, int inputIndex, double contrast = 1);
double combin_compare_9(Mat src_input, int inputIndex);
double combin_compare_10(Mat src_input, int inputIndex, double contrast = 1.2);
double combin_compare_11(Mat src_input, int inputIndex, double contrast = 1.2);
double combin_compare_12(Mat src_input, int inputIndex, double contrast = 1.2);
double combin_compare_13(Mat src_input, int inputIndex);
double combin_compare_14(Mat src_input, int inputIndex, double contrast = 1.12);
double combin_compare_15(Mat src_input, int inputIndex, double contrast = 1.12);
double combin_compare_16(Mat src_input, int inputIndex, double contrast = 1.12);
double combin_compare_17(Mat src_input, int inputIndex, double contrast = 1.12);
double combin_compare_18(Mat src_input, int inputIndex, double contrast = 1.12);