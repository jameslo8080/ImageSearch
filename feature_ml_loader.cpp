#include "feature_loader.h"

vector<pair<int, Mat> > load_mlSample(BOWImgDescriptorExtractor& bowExtractor, string featureName, int dictionarySize) {
	vector<pair<int, Mat> > res;
	vector<int> ids;
	vector<Mat> features;

	string fname = "../bow/f" + featureName + to_string(dictionarySize) + ".yml";
	FileStorage storage = FileStorage(fname, FileStorage::READ);

	if (storage["features"].empty()){
		FileStorage ws = FileStorage(fname, FileStorage::WRITE);

		cout << "Calculating features" << endl;
		vector<Mat> imgs = load_imgs();
		for (int i = 0; i < imgs.size(); ++i) {
			ids.push_back(i / 100);
			Mat descriptor = cal_descriptor(bowExtractor, imgs[i], featureName);
			features.push_back(descriptor);
		}
		ws << "ids" << ids;
		ws << "features" << features;
		ws.release();
	}
	else {
		storage["ids"] >> ids;
		storage["features"] >> features;
	}
	storage.release();

	for (int i = 0; i < ids.size(); ++i) {
		res.push_back(make_pair(ids[i], features[i]));
	}

	return res;
}
