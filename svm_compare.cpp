#include "compare.h"


CvSVM svm;
bool svmtrained = false;
bool greyscale = true;

void train_SVM(string featureName, int dictionarySize, BOWImgDescriptorExtractor bowExtractor) {
	vector<Mat> allDescriptors = load_allDescriptions_YML(featureName, 10, greyscale);

	for (int i = 0; i < allDescriptors.size(); ++i) {
		Mat greyMat;
		cvtColor(allDescriptors[i], greyMat, CV_BGR2GRAY);
		allDescriptors[i] = greyMat;
	}

	cout << "Loading BOW" << endl;

	string fname = "../bow/" + featureName + to_string(dictionarySize) + ".yml";
	FileStorage storage = FileStorage(fname, FileStorage::READ);
	if (storage["dictionary"].empty()){
		cout << "Prebuilt BOW not exist. Creating now..." << endl;
		save_BOW(featureName, dictionarySize, greyscale);
		storage = FileStorage(fname, FileStorage::READ);
	}
	Mat dictionary;
	storage["dictionary"] >> dictionary;

	cout << "Set vocabulary" << endl;
	bowExtractor.setVocabulary(dictionary);

	cout << "load Samples" << endl;
	vector<pair<int, Mat> >  vtrainingData = load_mlSample(bowExtractor, featureName, dictionarySize, greyscale);
	 
	Mat labels(0, 1, CV_32FC1);
	Mat trainingData(0, dictionarySize, CV_32FC1);
	for (auto p : vtrainingData) {
		labels.push_back((float)p.first);
		trainingData.push_back(p.second);
	}

	cout << "Train svm" << endl;

	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::RBF;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.000001);

	svm.train_auto(trainingData, labels, cv::Mat(), cv::Mat(), params);
	svmtrained = true;
}

double svm_compare(Mat src_input, int index) {
	if (greyscale) {
		Mat greyMat;
		cvtColor(src_input, greyMat, CV_BGR2GRAY);
		src_input = greyMat;
	}

	string featureName = "SURF";
	int dictionarySize = 1000;

	Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(featureName);
	Ptr<DescriptorMatcher>  matcher = DescriptorMatcher::create("FlannBased");
	BOWImgDescriptorExtractor bowExtractor(extractor, matcher);
	
	if (!svmtrained) train_SVM(featureName, dictionarySize, bowExtractor);

	cout << "Predict" << endl;
	Mat src_descriptor = cal_descriptor(bowExtractor, src_input, featureName);
	float response = svm.predict(src_descriptor);

	cout << index << " | svm result: " << response << endl;
	return (int)response == index;
}
