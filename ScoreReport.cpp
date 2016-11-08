#include "ScoreReport.h"

string getGrade(int firstWrong, int p, int r){
	return (firstWrong > 0 && p >= 60 && r >= 60) ? "A++" :
		(firstWrong > 0 && (p >= 60 || r >= 60)) ? "B++" :
		(p >= 60 && r >= 60) ? "B-+" : "F";
}

int getGradeNum(int firstWrong, int p, int r){
	return (firstWrong > 0 && p >= 60 && r >= 60) ? 1 :
		(firstWrong > 0 && (p >= 60 || r >= 60)) ? 2 :
		(p >= 60 && r >= 60) ? 3 : 4;
}

ScoreReport::ScoreReport(vector<ImgScore>& scoreList, int inputIndex) : scoreList(scoreList), inputIndex(inputIndex) {
	this->scoreList = scoreList;
	this->inputIndex = inputIndex;

	for (size_t i = 0; i < scoreList.size(); i++) {
		
		if (isMatch(i))
			correct++;
		else if (firstWrong == 1000) {
			firstWrong = i;
		}

		double p = (double)correct / (i + 1.0) * 100.00;
		double r = (double)correct;
		// when >60 we can see it as 60, since 60% precision gets 20% of marks
		double cp = (p > 60) ? 60 : p;
		double cr = (r > 60) ? 60 : r;

		double pr = p*r;
		double cpr = cp*cr;

		if (cpr > bestCPR) {
			bestCPR = cpr;
			bestP = p;
			bestR = r;
			bestPR = pr;
			bestPRS = scoreList[i].score;
			bestPSindex = i;
		}
		if (p >= 60 && r >= 60) {
			pr60.push_back(i);
		}
		if (getGradeNum(firstWrong, p, r) == 1) {
			gradeAindex.push_back(i);
			if (isMatch(i)) {
				//update best grade A
				bestGradeAindex = i;
				bestGradeAP = p;
				bestGradeAR = r;
			}
		}

		if (i == 99) {
			acc100 = p;
		}

		if (correct == 100) {
			if (firstWrong == 1000) firstWrong = i + 1;
			break;
		}
	}
}

void ScoreReport::report() {
	string grade = getGrade(firstWrong, bestP, bestR);
	printf("img#%i, Grade(%s) Acc100: %.0f%%, firstWrong:%i, bestIndex:%u, bP:%.4f%%, bR:%.4f%%, bPRS:%.4f \n"
		, inputIndex, grade.c_str(), acc100, firstWrong, bestPSindex, bestP, bestR, bestPRS);

	reportExtend();
}

void ScoreReport::report(int row, int col) {
	string grade = getGrade(firstWrong, bestP, bestR);
	printf("img#%i r%02ic%02i, Grade(%s) Acc100: %.0f%%, firstWrong:%i, bestIndex:%u, bP:%.4f%%, bR:%.4f%%, bPRS:%.4f  \n"
		, inputIndex, row, col, grade.c_str(), acc100, firstWrong, bestPSindex, bestP, bestR, bestPRS);

	reportExtend();
}

void ScoreReport::reportExtend() {
	reportPR60();
	reportGAindex();
	if (pr60.size() > 0 || gradeAindex.size() > 0) {
		cout << endl;
	}
}

void ScoreReport::reportPR60() {
	if (pr60.size() > 0) {
		printf(" --pr60 index (%i): ", pr60.size());
		for (int i = 0; i < pr60.size(); ++i)
			std::cout << pr60[i] << ' ';
		cout << endl;
	}
}
void ScoreReport::reportGAindex() {
	if (gradeAindex.size() > 0) {
		printf(" --grade A index (%i): ", gradeAindex.size());
		for (int i = 0; i < gradeAindex.size(); ++i)
			std::cout << gradeAindex[i] << ' ';
		cout << endl;

		int index = gradeAindex[gradeAindex.size() - 1];
		if (bestGradeAindex != -1) {
			printf(" --best grade A index:%3i score: %f ", bestGradeAindex, scoreList[index].score);
			printf(", P:%.4f%%, R:%.4f%%\n", bestGradeAP, bestGradeAR);
		}
	}
}
void ScoreReport::reportSorted(int count) {
	string files[] = { "man", "beach", "building", "bus", "dinosaur", "elephant", "flower", "horse", "mountain", "food" };
	printf("Sorted result (%s: %3i ~ %3i):", files[inputIndex].c_str(), inputIndex * 100, (inputIndex + 1) * 100 - 1);

	int times = (scoreList.size() > count) ? count : scoreList.size();
	for (int i = 0; i < times; i++) {
		if (i % 3 == 0)
			cout << endl;
		printf(" index:%3i, img#%3i, MScore:%.6f |", i, id(i), score(i));
		(isMatch(i)) ? cout << " T |" : cout << "   |";
	}
	printf("\n");
}