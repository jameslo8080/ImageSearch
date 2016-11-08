#include "core_cmp.h"


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

double getPSNR_divided(const vector<Mat>& I1, const vector<Mat>& I2, int col, int row) {
	int tot = col*row;
	
	double res = 0;
	for (int i = 0; i < tot; ++i) res += getPSNR(I1[i], I2[i]);
	return res;
}
