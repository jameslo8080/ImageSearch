#include "feature_compare.h"


inline int get_group(int db_id) {
	return (db_id < 0 || db_id > 999) ? -1 : db_id / 100;
}

double validate_fit(vector<ImgScore> ids, int target_id) {
	int cnt = 0;
	int len = ids.size();
	if (len == 0) return 0;

	for (auto imgS : ids) {
		if (get_group(imgS.db_id) == target_id) cnt++;
		cout << "db_id: " << imgS.db_id << "  Group: " << get_group(imgS.db_id) << "  score: " << imgS.score << endl;
	}
	return cnt / (double)len;
}


double descriptors_cal_match(Mat descriptors_1, Mat descriptors_2) {
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

	double max_dist = 0; double min_dist = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

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
