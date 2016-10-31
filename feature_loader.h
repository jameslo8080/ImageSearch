#pragma once 

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "feature_extract.h"

using namespace std;
using namespace cv;

#define SURF_LIST_FILE "inputsurf.txt"
#define IMAGE_LIST_FILE "inputimage.txt"  // NOTE: this is relative to current file

Mat load_allDescriptions();

void save_allDescriptions_YML();
vector<Mat> load_allDescriptions_YML();

Mat cal_descriptor(BOWImgDescriptorExtractor& bowExtractor, const Mat &db_img);

map<int, Mat> load_mlSample(BOWImgDescriptorExtractor& bowExtractor);

vector<Mat> load_features();
vector<Mat> load_imgs();
