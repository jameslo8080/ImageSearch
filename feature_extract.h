#pragma once 


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "feature_loader.h"

using namespace std;
using namespace cv;

vector<Mat> divide_image(Mat raw_img, int n_row, int n_col);
Mat rgbMat_to_hsvHist(Mat src_base);
vector<Mat> rgbMat_to_divided_hsvHist(Mat src_base, int n_row, int n_col);
Mat calSURFDescriptor(Mat input);
Mat calSIFTDescriptor(Mat input);
Mat calORBDescriptor(Mat input);

Mat cal_descriptor(BOWImgDescriptorExtractor bowExtractor, Mat db_img, string type);