#include "ml.h"


Mat trainBOW(vector<Mat> features, int dictionarySize) {
	TermCriteria tc(CV_TERMCRIT_ITER, 10, 0.001);
	int retries = 1;
	int flags = KMEANS_PP_CENTERS;

	BOWKMeansTrainer bowTrainer(dictionarySize, tc, retries, flags);
	for (auto feature : features) {
		Mat real = feature;
		bowTrainer.add(real);
	}

	return bowTrainer.cluster();
}
