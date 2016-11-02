#pragma once 

#include <iostream>
#include <vector>
#include "opencv2/features2d.hpp"
#include "ScoreReport.h"
using namespace std;
using namespace cv;


double validate_fit(vector<ImgScore> ids, int target_id);
double descriptors_cal_match(Mat descriptors_1, Mat descriptors_2);
