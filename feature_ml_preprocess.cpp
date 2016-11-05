#include "feature_preprocess.h"

void save_BOW(string featureName, int dictionarySize, bool greyScale) {

	vector<Mat> allDescriptors = load_allDescriptions_YML(featureName, 10, greyScale);

	Mat mallDescriptors;

	cout << "Train BOW" << endl;
	Mat dictionary = trainBOW(allDescriptors, dictionarySize);


	FileStorage fs("../bow/" + featureName + (greyScale?"g_":"") +to_string(dictionarySize) + ".yml", FileStorage::WRITE);
	fs << "dictionary" << dictionary;
	fs.release();

}
