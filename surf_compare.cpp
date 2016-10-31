#include "compare.h"


void surf_compare(Mat src_input, int index) {

	string files[] = { "man", "beach", "building", "bus", "dinosaur", "elephant", "flower", "horse", "mountain", "food" };
	//need to call save_allDescriptions_YML() once, before run this
	vector<Mat> allDescriptors = load_allDescriptions_YML();

	Mat inputDescriptor = calSURFDescriptor(src_input);
	vector<ImgScore> scores;

	// "../surf_c/"
	string compare_filepath = "../surf_c/" + files[index] + ".yml";
	cout << "load pre-compare : " << compare_filepath << endl;

	FileStorage fsload(compare_filepath, FileStorage::WRITE);
	if (fsload["s999"].empty()){
		fsload.release();
		FileStorage fssave(compare_filepath, FileStorage::WRITE);

		for (int i = 0; i < allDescriptors.size(); ++i) {
			ImgScore sc;
			sc.db_id = i;
			cout << "comparing : " << i << ".jpg" << endl;
			sc.score = surf_compare(inputDescriptor, allDescriptors[i]);

			string temp = "s" + std::to_string(i);
			fssave << temp << sc.score;

			cout << temp << ".jpg score : " << sc.score << endl;
			scores.push_back(sc);
		}
		fssave.release();
		sort(scores.rbegin(), scores.rend());

		scores.resize(10);
		printf("res0 Acc: %lf \n", validate_fit(scores, index));
		printf("Done \n");


	}
	else{
		// read score only
	}
}