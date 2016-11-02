#pragma once 

#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <map>
#include <algorithm>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"

#include "ml.h"


string getGrade(int firstWrong, int p, int r);

int getGradeNum(int firstWrong, int p, int r);


struct ImgScore{
	int db_id;
	double score;
	ImgScore() {}
	ImgScore(int i, double s) : db_id(i), score(s) {}
	bool operator < (const ImgScore& b) const {
		return score < b.score;
	}
};

// 𝑃𝑟𝑒𝑐𝑖𝑠𝑖𝑜𝑛 = 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠 𝑡ℎ𝑎𝑡 𝑎𝑟𝑒 𝑓𝑟𝑜𝑚 𝑡ℎ𝑒 𝑐𝑜𝑟𝑟𝑒𝑐𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 / 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑒𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠
// 𝑅𝑒𝑐𝑎𝑙𝑙 = 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠 𝑡ℎ𝑎𝑡 𝑎𝑟𝑒 𝑓𝑟𝑜𝑚 𝑡ℎ𝑒 𝑐𝑜𝑟𝑟𝑒𝑐𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 / 𝑡𝑜𝑡𝑎𝑙 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑖𝑚𝑎𝑔𝑒𝑠 𝑖𝑛 𝑡ℎ𝑒 𝑡𝑎𝑟𝑔𝑒𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 𝑜𝑓 𝑡ℎ𝑒 𝑑𝑎𝑡𝑎𝑠𝑒𝑡
struct ScoreReport{
public:
	vector<ImgScore> scoreList;
	ScoreReport(vector<ImgScore>& scoreList, int inputIndex) : scoreList(scoreList), inputIndex(inputIndex){ init(); }

	int inputIndex;
	int correct = 0; int firstWrong = 1000;
	double bestP = 0, bestR = 0, bestPR = 0, bestCPR = 0;
	double bestPRS = 0;
	int bestPSindex = -1;
	double acc100 = 0;
	vector<int> pr60;
	vector<int> gradeAindex;
	double lastGAP = 0, lastGAR = 0;
private:
	void init(){
		for (size_t i = 0; i < scoreList.size() && correct != 100; i++){

			if (scoreList[i].db_id / 100 == inputIndex)
				correct++;
			else if (firstWrong == 1000){
				firstWrong = i;
			}

			double p = (double)correct / (i + 1.0) * 100.00;
			double r = (double)correct;
			// when >60 we can see it as 60, since 60% precision gets 20% of marks
			double cp = (p > 60) ? 60 : p;
			double cr = (r > 60) ? 60 : r;

			double pr = p*r;
			double cpr = cp*cr;

			if (cpr >= bestCPR){
				bestCPR = cpr;
				bestP = p;
				bestR = r;
				bestPR = pr;
				bestPRS = scoreList[i].score;
				bestPSindex = i;
			}
			if (p >= 60 && r >= 60){
				pr60.push_back(i);
			}
			if (getGradeNum(firstWrong, p, r) == 1){
				gradeAindex.push_back(i);
				lastGAP = p;
				lastGAR = r;
			}
			if (i == 99){
				acc100 = p;
			}
		}
	}
public:
	void report(){
		string grade = getGrade(firstWrong, bestP, bestR);
		printf("img#%i, Grade(%s) Acc(100): %.4f%%, bestIndex:%u, bP:%.4f%%, bR:%.4f%%, bPRS:%.4f, firstWrong:%i \n"
			, inputIndex, grade.c_str(), acc100, bestPSindex, bestP, bestR, bestPRS, firstWrong);

		reportExtend();
	}

	void report(int row, int col){
		string grade = getGrade(firstWrong, bestP, bestR);
		printf("img#%i r%02ic%02i, Grade(%s) Acc(100): %.4f%%, bestIndex:%u, bP:%.4f%%, bR:%.4f%%, bPRS:%.4f, firstWrong:%i \n"
			, inputIndex, row, col, grade.c_str(), acc100, bestPSindex, bestP, bestR, bestPRS, firstWrong);

		reportExtend();
	}

	void reportExtend(){
		reportPR60();
		reportLGA();
		if (pr60.size() > 0 || gradeAindex.size() > 0){
			cout << endl;
		}
	}
	void reportPR60(){
		if (pr60.size() > 0){
			printf(" --pr60 index (%i): ", pr60.size());
			for (int i = 0; i < pr60.size(); ++i)
				std::cout << pr60[i] << ' ';
			cout << endl;
		}
	}
	void reportLGA(){
		if (gradeAindex.size() >0){
			printf(" --grade A index (%i): ", gradeAindex.size());
			for (int i = 0; i < gradeAindex.size(); ++i)
				std::cout << gradeAindex[i] << ' ';
			cout << endl;

			int index = gradeAindex[gradeAindex.size() - 1];
			cout << " --last grade A index score: " << scoreList[index].score;
			printf(", P:%.4f%%, R:%.4f%%\n", lastGAP, lastGAR);
		}
	}
};