#include "ScoreReport.h"

string getGrade(int firstWrong, int p, int r){
	return (firstWrong > 0 && p > 60 && r > 60) ? "A++" :
		(firstWrong > 0 && (p > 60 || r > 60)) ? "B++" :
		(p > 60 && r > 60) ? "B-+" : "F";
}

int getGradeNum(int firstWrong, int p, int r){
	return (firstWrong > 0 && p > 60 && r > 60) ? 1 :
		(firstWrong > 0 && (p > 60 || r > 60)) ? 2 :
		(p > 60 && r > 60) ? 3 : 4;
}