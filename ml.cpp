#include "ml.h"


Mat trainBOW(Mat allDescriptors) {
	const int wordCount = 1000;
	BOWKMeansTrainer bowTrainer(wordCount);
	Mat vocabulary = bowTrainer.cluster(allDescriptors);
	return vocabulary;
}

void trainSvm(const map<int, Mat> &samples, const int category, const CvSVMParams& svmParams, CvSVM* svm) {
	Mat allSamples(0, samples.at(category).cols, samples.at(category).type());
	Mat responses(0, 1, CV_32SC1);
	allSamples.push_back(samples.at(category));
	Mat posResponses(samples.at(category).rows, 1, CV_32SC1, Scalar::all(1));
	responses.push_back(posResponses);
	for (auto itr = samples.begin(); itr != samples.end(); ++itr) {
		if (itr->first == category) {
			continue;
		}
		allSamples.push_back(itr->second);
		Mat response(itr->second.rows, 1, CV_32SC1, Scalar::all(-1));
		responses.push_back(response);
	}
	svm->train(allSamples, responses, Mat(), Mat(), svmParams);
}

int classifyBySvm(map<int, Ptr<SVM>>& svms, const Mat& queryDescriptor) {
	float confidence = -2.0f;
	int category;
	for (auto itr = svms.begin(); itr != svms.end(); ++itr) {
		CvSVM *svm = itr->second;
		float curConfidence = svm->predict(queryDescriptor, true);
		if (curConfidence > confidence) {
			confidence = curConfidence;
			category = itr->first;
		}
	}
	return category;
}