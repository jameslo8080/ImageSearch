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

using namespace std;
using namespace cv;


int get_group(int db_id);
Mat convert_2DMat_to_1DMat(Mat inMat);
string BOW_file_path(string featureName, int dictionarySize, bool greyscale);
string ids_features_file_path(string featureName, int dictionarySize, bool greyscale);
