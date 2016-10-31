#include "feature_compare.h"

/*
man			: 0 ~ 99, 100?
beach		: 101 ~ 199, 100?
building	: 200 ~ 299
bus			: 300 ~ 399
dinosaur	: 400 ~ 499
elephant	: 500 ~ 599
flower		: 600 ~ 699
horse		: 700 ~ 799
mountain	: 800 ~ 899
food		: 900 ~ 999
*/



inline int get_group(int db_id) {
	return (db_id < 0 || db_id > 999)?-1:db_id/100;
}

bool isMatch(const ImgScore& imgsc, int targetClass){
	//return targetClass == imgsc.classID();

	switch (targetClass){
	case 0:
		if (imgsc.db_id >= 100 && imgsc.db_id <= 199)
			return true;
		break;
	case 1:
		if (imgsc.db_id >= 200 && imgsc.db_id <= 299)
			return true;
		break;
	case 2:
		if (imgsc.db_id >= 300 && imgsc.db_id <= 399)
			return true;
		break;
	case 3:
		if (imgsc.db_id >= 400 && imgsc.db_id <= 499)
			return true;
		break;
	case 4:
		if (imgsc.db_id >= 600 && imgsc.db_id <= 699)
			return true;
		break;
	case 5:
		if (imgsc.db_id >= 700 && imgsc.db_id <= 799)
			return true;
		break;
	case 6:
		if (imgsc.db_id >= 0 && imgsc.db_id <= 100)
			return true;
		break;
	}
	return false;

}

double validate_fit(vector<ImgScore> ids, int target_id) {
	int cnt = 0;
	int len = ids.size();
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

	printf("B,");
	matcher.match(descriptors_1, descriptors_2, matches);

	printf("C,");
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
