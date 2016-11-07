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
#include "feature_loader.h"
#include "ml.h"

using namespace std;
using namespace cv;

void save_allSURFDescriptions_YML(bool greyscale = false);
void save_allSIFTDescriptions_YML(bool greyscale = false);
void save_allORBDescriptions_YML(bool greyscale = false);
void save_BOW(string featureName, int dictionarySize = 1000, bool greyscale = false, int partSize = 10);
