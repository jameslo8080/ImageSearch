#include "utils.h"


// "../image.orig/685.jpg"
string getFilePath999(int index) {
	if (index < 0 || index > 999)
		index = 0;
	string s("../image.orig/");
	s.append(std::to_string(index) + ".jpg");
	return s;
}

int get_group(int db_id) {
	return (db_id < 0 || db_id > 999) ? -1 : db_id / 100;
}

Mat convert_2DMat_to_1DMat(Mat inMat) {
	Mat outMat(1, inMat.rows*inMat.cols, CV_32F);

	int ii = 0;
	for (int i = 0; i < inMat.rows; i++) {
		for (int j = 0; j < inMat.cols; j++) {
			outMat.at<float>(0, ii++) = inMat.at<uchar>(i, j);
		}
	}

	return outMat;
}


double validate_fit(vector<ImgScore> ids, int target_id) {
	int cnt = 0;
	int len = ids.size();
	if (len == 0) return 0;

	for (auto imgS : ids) if (get_group(imgS.db_id) == target_id) cnt++;

	return cnt / (double)len;
}


vector<double> normalize_vec(const vector<double> &vec) {
	int len = vec.size();
	if (len == 0) return {};
	if (len == 1) return { 0 };

	vector<double> res(len);
	double minv = vec[0], maxv = vec[0];
	for (int i = 1; i < len; ++i) {
		if (vec[i] < minv) minv = vec[i];
		if (vec[i] > maxv) maxv = vec[i];
	}

	int range = maxv - minv;
	for (int i = 0; i < len; ++i) {
		res[i] = range == 0 ? 0 : (vec[i] - minv) / range;
	}

	return res;
}

string BOW_file_path(string featureName, int dictionarySize, bool greyscale) {
	return "../bow/" + (string)(greyscale ? "g_" : "") + featureName + to_string(dictionarySize) + ".yml";
}

string ids_features_file_path(string featureName, int dictionarySize, bool greyscale) {
	return "../bow/" + (string)(greyscale ? "g_f" : "f") + featureName + to_string(dictionarySize) + ".yml";
}

void save_result(const vector<int>& nums) {
	wstring fpath = L"../result/";
	RemoveDirectory(fpath.data());
	CreateDirectory(fpath.data(), NULL);
	for (auto num : nums) {
		ifstream src(getFilePath999(num), ios::binary);
		ofstream dst("../result/" + to_string(num) + ".jpg", ios::binary);
		dst << src.rdbuf();
	}
}