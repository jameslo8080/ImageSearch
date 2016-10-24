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

double validate_fit(vector<ImgScore> ids, int target_id) {
	int cnt = 0;
	int len = ids.size();
	for (auto imgS : ids) {
		if (get_group(imgS.db_id) == target_id) cnt++;
		cout << "db_id: " << imgS.db_id << "  Group: " << get_group(imgS.db_id) << "  score: " << imgS.score << endl;
	}
	return cnt / (double)len;
}
