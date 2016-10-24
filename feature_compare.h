#pragma once 

#include <iostream>
#include <vector>
using namespace std;

struct ImgScore{
	int db_id;
	double score;
	ImgScore() {}
	ImgScore(int i, double s) : db_id(i), score(s) {}
	bool operator < (const ImgScore& b) const {
		return score < b.score;
	}
};

double validate_fit(vector<ImgScore> ids, int target_id);
