#include "compare.h"


CvSVM svm;
bool svmtrained = false;


double svm_compare(Mat src_input, int index) {
	bool greyscale = true;
	string featureName = "SURF";
	int dictionarySize = 1000; 
	
	Mat testMat;
	if (greyscale) {
		cvtColor(src_input, testMat, CV_BGR2GRAY);
	}
	else {
		testMat = src_input;
	}

	Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(featureName);
	Ptr<DescriptorMatcher>  matcher = DescriptorMatcher::create("FlannBased");
	BOWImgDescriptorExtractor bowExtractor(extractor, matcher);

	cout << "Loading BOW" << endl;
	string fname = BOW_file_path(featureName, dictionarySize, greyscale);
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
	params.kernel_type = CvSVM::RBF; // LINEAR POLY RBF SIGMOID
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.000001);

	//svm.train(trainingData, labels, cv::Mat(), cv::Mat(), params);
	if(!svmtrained) svm.train_auto(trainingData, labels, cv::Mat(), cv::Mat(), params);
	svmtrained = true;

	cout << "Predict" << endl;
	Mat src_descriptor = cal_descriptor_bow(bowExtractor, testMat, featureName);
	float response = svm.predict(src_descriptor);

	cout << index << " | svm result: " << response << endl;
	return (int)response == index;
}
