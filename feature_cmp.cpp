#include "core_cmp.h"

double feature_cmp(const Mat& descriptors_1, const Mat& descriptors_2) {
	if (descriptors_1.type() != CV_32F) {
		descriptors_1.convertTo(descriptors_1, CV_32F);
	}
	if (descriptors_2.type() != CV_32F) {
		descriptors_2.convertTo(descriptors_2, CV_32F);
	}

	if (descriptors_1.empty())
		cout << "1st descriptor empty" << endl;
	if (descriptors_2.empty())
		cout << "2st descriptor empty" << endl;

	BFMatcher matcher(NORM_L2);
	// BFMatcher matcher(NORM_HAMMING);
	// FlannBasedMatcher matcher;
	vector< DMatch > matches;

	matcher.match(descriptors_1, descriptors_2, matches);

	sort(matches.begin(), matches.end());
	vector< DMatch > good_matches;
	double res = 0.0;
	for (int i = 0; i < matches.size(); i++)
	{
		good_matches.push_back(matches[i]);
		res += matches[i].distance;
		if (good_matches.size() >= 100) break;
	}

	if (good_matches.size() == 0) return 9999;
	return res / (double)good_matches.size();
}