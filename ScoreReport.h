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


struct ImgScore {
		int db_id;
		double score;
		ImgScore() {}
		ImgScore(int i, double s) : db_id(i), score(s) {}
		bool operator < (const ImgScore& b) const { return score < b.score; }
};

// 𝑃𝑟𝑒𝑐𝑖𝑠𝑖𝑜𝑛 = 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠 𝑡ℎ𝑎𝑡 𝑎𝑟𝑒 𝑓𝑟𝑜𝑚 𝑡ℎ𝑒 𝑐𝑜𝑟𝑟𝑒𝑐𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 / 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑒𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠
// 𝑅𝑒𝑐𝑎𝑙𝑙 = 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑟𝑒𝑡𝑟𝑖𝑣𝑒𝑑 𝑖𝑚𝑎𝑔𝑒𝑠 𝑡ℎ𝑎𝑡 𝑎𝑟𝑒 𝑓𝑟𝑜𝑚 𝑡ℎ𝑒 𝑐𝑜𝑟𝑟𝑒𝑐𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 / 𝑡𝑜𝑡𝑎𝑙 𝑛𝑢𝑚𝑏𝑒𝑟 𝑜𝑓 𝑖𝑚𝑎𝑔𝑒𝑠 𝑖𝑛 𝑡ℎ𝑒 𝑡𝑎𝑟𝑔𝑒𝑡 𝑐𝑎𝑡𝑒𝑔𝑜𝑟𝑦 𝑜𝑓 𝑡ℎ𝑒 𝑑𝑎𝑡𝑎𝑠𝑒𝑡

class ScoreReport {
public:
		vector<ImgScore> scoreList;
		ScoreReport() {}
		ScoreReport(vector<ImgScore>& scoreList, int inputIndex);

		int inputIndex;
		int correct = 0; int firstWrong = 1000;
		double bestP = 0, bestR = 0, bestPR = 0, bestCPR = 0;
		double bestPRS = 0;
		int bestPSindex = -1;
		double acc100 = 0;
		vector<int> pr60;
		vector<int> gradeAindex;
		int bestGradeAindex = -1;
		double bestGradeAP = 0, bestGradeAR = 0;

public:
		void report();
		void report(int row, int col);
		void reportExtend();
		void reportPR60();
		void reportGAindex();
		void reportSorted(int count);

		int id(int index) { return scoreList[index].db_id; }
		double score(int index) { return scoreList[index].score; }
		double bestGradeAPR() { return bestGradeAP * bestGradeAR; }
		bool isMatch(int index) { return scoreList[index].db_id / 100 == inputIndex; }

		pair<double, double> getPRbyScore(double s) {
				int correct = 0;
				for (int i = 0; i < scoreList.size(); i++) {
						if (score(i) >= s) {
								//keep searching
								if (isMatch(i))
										correct++;
						} else {
								//finish
								double p;
								if (i > 0)
										p = (double) correct / (double) (i) * 100.0;
								else
										p = 0;
								double r = (double) correct;
								return{ p, r };
								break;
						}
				}
				double p = (double) correct / (double) 1000 * 100.0;
				double r = (double) correct;
				return{ p, r };
		}

};