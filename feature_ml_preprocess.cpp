#include "feature_preprocess.h"

void save_BOW(string featureName, int dictionarySize, bool greyScale) {

	vector<Mat> allDescriptors = load_allDescriptions_YML(featureName, 10, greyScale);

	Mat mallDescriptors;

	cout << "Train BOW" << endl;
	Mat dictionary = trainBOW(allDescriptors, dictionarySize);

	string fname = BOW_file_path(featureName, dictionarySize, greyScale);
	FileStorage fs(fname, FileStorage::WRITE);
	fs << "dictionary" << dictionary;
	fs.release();

}
