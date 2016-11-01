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


int surf_compare(Mat descriptors_1, Mat descriptors_2) {
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

	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;

	matcher.match(descriptors_1, descriptors_2, matches);

	double max_dist = 0; double min_dist = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (matches[i].distance <= max(2 * min_dist, 0.02))
		{
			good_matches.push_back(matches[i]);
		}
	}

	return good_matches.size();
}
