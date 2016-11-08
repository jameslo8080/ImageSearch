#include "feature_loader.h"

vector<pair<int, Mat> > load_mlSample(BOWImgDescriptorExtractor& bowExtractor, string featureName, int dictionarySize, bool greyscale) {
	vector<pair<int, Mat> > res;
	vector<int> ids;
	vector<Mat> features;

	string fname = "../bow/f" + featureName + to_string(dictionarySize) + ".yml";
	FileStorage storage = FileStorage(fname, FileStorage::READ);

	if (storage["features"].empty()){
		FileStorage ws = FileStorage(fname, FileStorage::WRITE);

		vector<Mat> imgs = load_imgs();
		cout << "Calculating features" << endl;
		for (int i = 0; i < imgs.size(); ++i) {
			ids.push_back(i / 100);
			Mat descriptor = cal_descriptor_bow(bowExtractor, imgs[i], featureName);

			if (greyscale) {
				Mat greyMat;
				cvtColor(descriptor, greyMat, CV_BGR2GRAY);
				descriptor = greyMat;
			}

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
