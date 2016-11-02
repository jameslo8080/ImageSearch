#include "compare.h"

double svm_compare(Mat src_input, int index) {
	//Too slow in training, so give up

	vector<Mat> allDescriptors = load_allDescriptions_YML("SIFT");
	Mat mallDescriptors;
	for (auto m : allDescriptors) mallDescriptors.push_back(m);
	cout << "Train BOW" << endl;
	Mat dictionary = trainBOW(mallDescriptors);
	Ptr<DescriptorExtractor> extractor = DescriptorMatcher::create("SIFT");
	Ptr<DescriptorMatcher>  matcher = DescriptorExtractor::create("BruteForce");
	BOWImgDescriptorExtractor bowExtractor(extractor, matcher);
	bowExtractor.setVocabulary(dictionary);
	cout << "load Samples" << endl;
	map<int, Mat> samples = load_mlSample(bowExtractor);
	map<int, Ptr<SVM>> svms;
	cout << "train Svm" << endl;
	for (int i = 0; i < 10; ++i) {
		CvSVMParams svmParams;
		svms[i] = new SVM;
		trainSvm(samples, i, svmParams, svms[i]);
		string fname = to_string(i);
		string extension(".xml");
		string fullname = fname + extension;
		svms[i]->save(fullname.c_str());
	}
	cout << "Predict" << endl;
	Mat src_descriptor = cal_descriptor(bowExtractor, src_input);
	int result = classifyBySvm(svms, src_descriptor);
	cout << "svm result " << result << endl;
	return 0;
}