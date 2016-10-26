#pragma once 


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"

using namespace std;
using namespace cv;


Mat rgbMat_to_hsvHist(Mat src_base);
Mat calSURFDescriptor(Mat input);