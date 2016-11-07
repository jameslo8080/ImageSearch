#include "compare.h"

double _compare(Mat inputDescriptor, vector<Mat> allDescriptors, int index) {

	string files[] = { "man", "beach", "building", "bus", "dinosaur", "elephant", "flower", "horse", "mountain", "food" };
	vector<ImgScore> scores;

	// "../surf_c/"
	string compare_filepath = "../surf_c/" + files[index] + ".yml";

	//FileStorage fsload(compare_filepath, FileStorage::WRITE);

	if (true) { //fsload["s999"].empty()){
		//fsload.release();
		FileStorage fssave(compare_filepath, FileStorage::WRITE);

		for (int i = 0; i < allDescriptors.size(); ++i) {
			ImgScore sc;
			sc.db_id = i;
			sc.score = feature_cmp(inputDescriptor, allDescriptors[i]);

			string temp = "s" + to_string(i);
			fssave << temp << sc.score;
			scores.push_back(sc);
		}
		fssave.release();
		sort(scores.begin(), scores.end());

		scores.resize(100);

		double acc = validate_fit(scores, index);

		ScoreReport sr(scores, index);
		sr.report();
		return acc;

	}
	else{
		cout << "load pre-compare : " << compare_filepath << endl;
		return 0;
	}
}

// save_allSURFDescriptions_YML
double surf_compare(Mat src_input, int index) {
	Mat inputDescriptor = calSURFDescriptor(src_input);
	vector<Mat> allDescriptors = load_allDescriptions_YML("SURF");
	return _compare(inputDescriptor, allDescriptors, index);
}

// save_allSIFTDescriptions_YML
double sift_compare(Mat src_input, int index) {
	Mat inputDescriptor = calSIFTDescriptor(src_input);
	vector<Mat> allDescriptors = load_allDescriptions_YML("SIFT");
	return _compare(inputDescriptor, allDescriptors, index);
}

// save_allORBDescriptions_YML
double orb_compare(Mat src_input, int index) {
	Mat inputDescriptor = calORBDescriptor(src_input);
	vector<Mat> allDescriptors = load_allDescriptions_YML("ORB");
	return _compare(inputDescriptor, allDescriptors, index);
}

