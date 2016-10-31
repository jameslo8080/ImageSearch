
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
#include "ml.h"

using namespace std;
using namespace cv;



void hsv_compare(Mat src_input, int index);
void svm_compare(Mat src_input, int index);
void surf_compare(Mat src_input, int index);