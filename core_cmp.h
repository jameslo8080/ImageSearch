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

using namespace std;
using namespace cv;

double pixel_cmp(Mat img1, Mat img2);

double hist_cmp(const Mat& img1, const Mat& img2, int method = 0);
double hsv_cmp(const Mat& img1, const Mat& img2, int method = 0);
double hsv_splited_cmp(const Mat& img1, const Mat& img2, int row = 2, int col = 2, int method = 0);
double hsv_splited_cmp(const vector<Mat>& splited_img1, const Mat& img2, int row = 2, int col = 2, int method = 0);
double hsv_splited_cmp(const vector<Mat>& splited_img1, const vector<Mat>& splited_img2, int row, int col, int method);

double getPSNR(const Mat& I1, const Mat& I2);
double getPSNR_divided(const vector<Mat>& I1, const vector<Mat>& I2, int col, int row);
Scalar getMSSIM(const Mat& i1, const Mat& i2);


double feature_cmp(const Mat& descriptors_1, const Mat& descriptors_2);

double contour_cmp(vector<vector<Point> > a, vector<vector<Point> > b);