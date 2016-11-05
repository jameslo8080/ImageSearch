#include "feature_loader.h"

vector<pair<int, Mat> > load_mlSample(BOWImgDescriptorExtractor& bowExtractor, string featureName, int dictionarySize, bool greyscale) {
	vector<pair<int, Mat> > res;
	vector<int> ids;
	vector<Mat> features;

	string fname = ids_features_file_path(featureName, dictionarySize, greyscale);
	FileStorage storage = FileStorage(fname, FileStorage::READ);

	if (storage["features"].empty()){
		FileStorage ws = FileStorage(fname, FileStorage::WRITE);

		vector<Mat> imgs = load_imgs();
		cout << "Calculating features" << endl;
		for (int i = 0; i < imgs.size(); ++i) {
			ids.push_back(i / 100);

			if (greyscale) {
				Mat greyMat;
				cvtColor(imgs[i], greyMat, CV_BGR2GRAY);
				imgs[i] = greyMat;
			}

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
