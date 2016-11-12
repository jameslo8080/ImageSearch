#pragma once 

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <map>
#include <algorithm>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "ScoreReport.h"

using namespace std;
using namespace cv;


string getFilePath999(int index);
int get_group(int db_id);
Mat convert_2DMat_to_1DMat(Mat inMat);


double validate_fit(vector<ImgScore> ids, int target_id);

vector<double> normalize_vec(const vector<double> &vec);

string BOW_file_path(string featureName, int dictionarySize, bool greyscale);
string ids_features_file_path(string featureName, int dictionarySize, bool greyscale);

void save_result(const vector<int>& nums, int inputIndex);
