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
#include "feature_compare.h"
#include "ScoreReport.h"
#include "ml.h"

using namespace std;
using namespace cv;



double hsv_compare(Mat src_input, int index);

double hsv_split_compare(Mat src_input, int index);
double hsv_split_compare(Mat src_input, int index, bool output, int row, int col);
double svm_compare(Mat src_input, int index);
double surf_compare(Mat src_input, int index);
double sift_compare(Mat src_input, int index);
double orb_compare(Mat src_input, int index);
