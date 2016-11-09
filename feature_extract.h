#pragma once 


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "feature_loader.h"

using namespace std;
using namespace cv;

Mat equalized(Mat src);

Mat greyscaleHist(const Mat& src_base);
vector<Mat> greyscaleHist(const vector<Mat>& imgs);

Mat rgbHist(const Mat& src_base);
vector<Mat> rgbHist(const vector<Mat>& src_base);

vector<Mat> divide_image(Mat raw_img, int n_row, int n_col);

Mat rgbMat_to_hsvHist(Mat src_base);
vector<Mat> rgbMat_to_divided_hsvHist(Mat src_base, int n_row, int n_col);

Mat contrast_brightness_change(const Mat& img, double alpha, double beta);

Mat crop_ellipse(const Mat& im1);

Mat calSURFDescriptor(Mat input);
Mat calSIFTDescriptor(Mat input);
Mat calORBDescriptor(Mat input);

Mat cal_descriptor_bow(BOWImgDescriptorExtractor& bowExtractor, const Mat &db_img, string type);
vector<vector<Point> > getCannyContours(const Mat& img, double thresh = 100);
