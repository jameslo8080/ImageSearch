#pragma once 

#include <vector>
#include <queue>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include "opencv2/features2d/features2d.hpp"

using namespace std;
using namespace cv;


Mat trainBOW(Mat allDescriptors);
void trainSvm(const map<int, Mat> &samples, const int category, const CvSVMParams& svmParams, CvSVM* svm);
int classifyBySvm(map<int, Ptr<SVM>>& svms, const Mat& queryDescriptor);