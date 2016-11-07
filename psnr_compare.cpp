#include "compare.h"


double getPSNR(const Mat& I1, const Mat& I2)
{
	Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2

	Scalar s = sum(s1);         // sum elements per channel

	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if (sse <= 1e-10) // for small values return zero
		return 0;
	else
	{
		double  mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0*log10((255 * 255) / mse);
		return psnr;
	}
}

double psnr_compare(Mat src_input, int inputIndex) {

	vector<Mat> features = load_features();
	Mat src_hsv = rgbMat_to_hsvHist(src_input);
	vector<ImgScore> imgScoreResult;
	for (int i = 0; i < features.size(); ++i) imgScoreResult.push_back(ImgScore(i, getPSNR(features[i], src_hsv)));

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return sr.acc100;
}
