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
#include "utils.h"

using namespace std;
using namespace cv;

#define ORB_LIST_FILE "inputorb.txt"
#define SIFT_LIST_FILE "inputsift.txt"
#define SURF_LIST_FILE "inputsurf.txt"
#define IMAGE_LIST_FILE "inputimage.txt"  // NOTE: this is relative to current file

Mat load_allDescriptions();

vector<Mat> load_allDescriptions_YML(string type, int partOf100 = 100, bool greyscale=false);

vector<pair<int, Mat>>  load_mlSample(BOWImgDescriptorExtractor& bowExtractor, string type, int dictionarySize, bool greyscale=false);

vector<Mat> load_hsvHist();
vector<Mat> load_imgs(bool out=false);
bool read_images(FILE* fp, Mat &db_img, int db_id);
