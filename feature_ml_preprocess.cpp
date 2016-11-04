#include "feature_preprocess.h"

void save_BOW(string featureName, int dictionarySize) {

	vector<Mat> allDescriptors = load_allDescriptions_YML(featureName, 10);

	Mat mallDescriptors;

	cout << "Train BOW" << endl;
	Mat dictionary = trainBOW(allDescriptors, dictionarySize);


	FileStorage fs("../bow/" + featureName + to_string(dictionarySize) + ".yml", FileStorage::WRITE);
	fs << "dictionary" << dictionary;
	fs.release();

}
