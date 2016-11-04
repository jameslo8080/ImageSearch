#pragma once 

#include <vector>
#include <queue>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include "opencv2/features2d/features2d.hpp"

using namespace std;
using namespace cv;


Mat trainBOW(vector<Mat> features, int dictionarySize);
