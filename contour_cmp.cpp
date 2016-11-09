#include "core_cmp.h"

double contour_cmp(vector<vector<Point> > a, vector<vector<Point> > b) {
	double cnt = 0;
	for (auto t : a) {
		bool match = false;
		for (auto t2 : b) {
			double comres = matchShapes(t, t2, CV_CONTOURS_MATCH_I3, 0.0);
			if (comres > 2) {
				match = true;
				break;
			}	
		}
		if (match) cnt++;
	}
	return cnt;
}
