#include "compare.h"

double svm_compare(Mat src_input, int index) {
	string featureName = "SIFT";
	int dictionarySize = 1500;

	vector<Mat> allDescriptors = load_allDescriptions_YML(featureName, 10);

	Mat mallDescriptors;

	cout << "Loading BOW" << endl;

	string fname = "../bow/" + featureName + to_string(dictionarySize) + ".yml";
	FileStorage storage = FileStorage(fname, FileStorage::READ);
	if (storage["dictionary"].empty()){
		cout << "Prebuilt BOW not exist. Creating now..." << endl;
		save_BOW(featureName, dictionarySize);
		storage = FileStorage(fname, FileStorage::READ);
	}
	Mat dictionary;
	storage["dictionary"] >> dictionary;

	cout << "Set vocabulary" << endl;
	Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(featureName);
	Ptr<DescriptorMatcher>  matcher = DescriptorMatcher::create("FlannBased");
	BOWImgDescriptorExtractor bowExtractor(extractor, matcher);
	bowExtractor.setVocabulary(dictionary);

	cout << "load Samples" << endl;
	vector<pair<int, Mat> >  vtrainingData = load_mlSample(bowExtractor, featureName, dictionarySize);
	Mat labels(0, 1, CV_32FC1);
	Mat trainingData(0, dictionarySize, CV_32FC1);
	for (auto p : vtrainingData) {
		labels.push_back((float)p.first);
		trainingData.push_back(p.second);
	}

	cout << "Train svm" << endl;
	CvSVMParams params;
	params.kernel_type = CvSVM::RBF;
	params.svm_type = CvSVM::C_SVC;
	params.gamma = 0.50625000000000009;
	params.C = 312.50000000000000;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.000001);
	CvSVM svm;

	bool res = svm.train(trainingData, labels, cv::Mat(), cv::Mat(), params);

	cout << "Predict" << endl;
	Mat src_descriptor = cal_descriptor(bowExtractor, src_input, featureName);

	float response = svm.predict(src_descriptor);

	cout << index << " | svm result: " << response << endl;
	return (int)response == index;
}