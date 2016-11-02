#include "compare.h"

double hsv_compare(Mat src_input, int inputIndex) {
	vector<Mat> features = load_features();
	Mat src_hsv = rgbMat_to_hsvHist(src_input);
	vector<ImgScore> imgScoreResult;
	for (int i = 0; i < features.size(); ++i) imgScoreResult.push_back(ImgScore(i, compareHist(features[i], src_hsv, 0)));

	sort(imgScoreResult.rbegin(), imgScoreResult.rend());
	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report();

	return 0;
}

double hsv_split_compare(Mat src_input, int inputIndex) {
	return  hsv_split_compare(src_input, inputIndex, true, 2, 2);
}

double hsv_split_compare(Mat src_input, int inputIndex, bool output, int row, int col) {
	vector<Mat> imgs = load_imgs();

	int tot = row*col;
	vector<Mat> src_hsv = rgbMat_to_divided_hsvHist(src_input, row, col);

	vector<ImgScore> imgScoreResult;

	for (int i = 0; i < imgs.size(); ++i) {
		vector<Mat> splited_img = rgbMat_to_divided_hsvHist(imgs[i], row, col);
		double scores = 0;
		double sums = 0;
		for (int i = 0; i < tot; ++i)
			sums += compareHist(splited_img[i], src_hsv[i], 0);
		imgScoreResult.push_back(ImgScore(i, sums / (double)(tot)));
	}

	// sort
	sort(imgScoreResult.rbegin(), imgScoreResult.rend());

	ScoreReport sr(imgScoreResult, inputIndex);
	sr.report(row, col);

	return 0;
}

//int correct = 0; int firstWrong = 1000;
//double bestP = 0, bestR = 0, bestPR = 0, bestCPR = 0;
//double bestPRS = 0;
//int bestPSindex = -1;
//double acc100 = 0;
//vector<int> pr60;
//vector<int> gradeAindex;
//double lastGAP = 0, lastGAR = 0;
//for (size_t i = 0; i < imgScoreResult.size() && correct != 100; i++)
//{

//	if (imgScoreResult[i].db_id / 100 == inputIndex)
//		correct++;
//	else if (firstWrong == 1000){
//		firstWrong = i;
//	}

//	double p = (double)correct / (i + 1) * 100.00;
//	double r = (double)correct / 100.00 * 100.00;
//	// when >60 we can see it as 60, since 60% precision gets 20% of marks
//	double cp = (p > 60) ? 60 : p;
//	double cr = (r > 60) ? 60 : r;

//	double pr = p*r;
//	double cpr = cp*cr;

//	if (cpr >= bestCPR){
//		bestCPR = cpr;
//		bestP = p;
//		bestR = r;
//		bestPR = pr;
//		bestPRS = imgScoreResult[i].score;
//		bestPSindex = i;
//	}
//	if (p >= 60 && r >= 60){
//		pr60.push_back(i);
//	}
//	if (getGradeNum(firstWrong, p, r) == 1){
//		gradeAindex.push_back(i);
//		lastGAP = p;
//		lastGAR = r;
//	}
//	if (i == 99){
//		acc100 = p;
//	}
//}
///*
//target:

//using setting:
//img:0 r:5, c:5, #1:T Acc(100): x.xx%, bestP: x.xx%(s:xx.xx), bestR: x.xx%(s:xx.xx)
//img:0 r:5, c:6, #1:F Acc(100): x.xx%,
//*/

////res.resize(100);
////res.erase(remove_if(res.begin(), res.end(), [](const ImgScore &x){return x.score < 0.3; }), res.end());
////double acc = validate_fit(res, index);

//string grade = getGrade(firstWrong, bestP, bestR);
//printf("img#%i r%02ic%02i, Grade(%s) Acc(100): %.4f%%, bestIndex:%u, bP:%.4f, bR:%.4f, bPRS:%.4f, firstWrong:%i \n"
//	, inputIndex, row, col, grade.c_str(), acc100, bestPSindex, bestP, bestR, bestPRS, firstWrong);

//if (pr60.size() > 0){
//	printf(" --pr60 index (%i): ", pr60.size());
//	for (int i = 0; i < pr60.size(); ++i)
//		std::cout << pr60[i] << ' ';
//	cout << endl;
//}

//if (gradeAindex.size() >0){
//	printf(" --grade A index (%i): ", gradeAindex.size());
//	for (int i = 0; i < gradeAindex.size(); ++i)
//		std::cout << gradeAindex[i] << ' ';
//	cout << endl;

//	int index = gradeAindex[gradeAindex.size() - 1];
//	cout << " --last grade A index score: " << imgScoreResult[index].score;
//	printf(", P:%.4f, R:%.4f\n", lastGAP, lastGAR);
//}

//if (pr60.size() > 0 || gradeAindex.size() > 0){
//	cout << endl;
//}