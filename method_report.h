#include "feature_extract.h"
#include "compare.h"
#include "ScoreReport.h"

//void getBestMethodResult(vector<MethodResult>) {
//
//}

// same setting for all input and method
struct MethodResult {
		int _minH = -1, _hbins = -1, _sbins = -1;
		int _hce_method_dbimg = -1;
		vector<double> other_setting;
		vector<double> _acc_of;
		vector<double> _acc_first_match;

		vector<double> best_threshold;
		vector<pair<double, double>> best_threshold_pr;

		vector<double> best_threshold_fm; //first match only
		vector<pair<double, double>> best_threshold_fm_pr;

		vector<int> _first_match_count;

		void add_best_threshold(vector<ScoreReport> sr_list_for_same_method) {
				double best_p = 0, best_r = 0;
				double best_s = 0;

				double best_p_a = 0, best_r_a = 0;

				double best_p_fm = 0, best_r_fm = 0;
				double best_s_fm = 0;
				double best_p_fma = 0, best_r_fma = 0;

				for each (auto sr in sr_list_for_same_method) { // for all score

						int correct = 0;
						for (size_t i = 0; i < sr.scoreList.size(); i++) {
								if (sr.isMatch(i)) {
										correct++;
								}

								double score = sr.score(i);

								double sum_p = 0, sum_r = 0;
								double sum_p_a = 0, sum_r_a = 0;

								double sum_p_fm = 0, sum_r_fm = 0;
								double sum_p_fm_a = 0, sum_r_fm_a = 0;
								for each (auto sr_sub in sr_list_for_same_method) {
										bool is_fm = sr_sub.firstWrong != 0;

										pair<double, double> pr = sr_sub.getPRbyScore(score);
										sum_p += (pr.first > 60) ? 60 : (double) pr.first;
										sum_r += (pr.second > 60) ? 60 : (double) pr.second;

										sum_p_a += (double) pr.first;
										sum_r_a += (double) pr.second;

										if (is_fm) {
												sum_p_fm += (pr.first > 60) ? 60 : (double) pr.first;
												sum_r_fm += (pr.second > 60) ? 60 : (double) pr.second;

												sum_p_fm_a += (double) pr.first;
												sum_r_fm_a += (double) pr.second;
										}
								}

								if (sum_p + sum_r > best_p + best_r) {
										best_p = sum_p;
										best_r = sum_r;
										best_s = score;

										best_p_a = sum_p_a;
										best_r_a = sum_r_a;
								}

								if (sum_p_fm + sum_r_fm > best_p_fm + best_r_fm) {
										best_p_fm = sum_p_fm;
										best_r_fm = sum_r_fm;
										best_s_fm = score;
										best_p_fma = sum_p_fm_a;
										best_r_fma = sum_r_fm_a;
								}


								if (correct == 100) break;
						}
						best_threshold.push_back(best_s);

						double fin_p = 0, fin_r = 0;
						double fin_p_fm = 0, fin_r_fm = 0;
						for each (auto sr_sub in sr_list_for_same_method) {
								pair<double, double> pr = sr_sub.getPRbyScore(best_s);
								fin_p += (double) pr.first;
								fin_r += (double) pr.second;

								if (sr_sub.firstWrong != 0) {
										pair<double, double> pr_fm = sr_sub.getPRbyScore(best_s_fm);
										fin_p_fm += (double) pr_fm.first;
										fin_r_fm += (double) pr_fm.second;
								}
						}
						best_threshold_pr.push_back({ fin_p, fin_r });

						best_threshold_fm.push_back(best_s_fm);
						best_threshold_fm_pr.push_back({ fin_p_fm, fin_r_fm });
				}


		}

		double getBestAccFirstMatch() {
				double best = 0;
				for each (auto var in _acc_first_match) { if (var > best) best = var; }
				return best;
		}

		double getBestAcc() {
				double best = 0;
				for each (auto var in _acc_of) { if (var > best) best = var; }
				return best;
		}
		void add_acc(double acc) {
				_acc_of.push_back(acc);
		}

		void add_double_setting(double setting) {
				other_setting.push_back(setting);
		}

		// sr_list is for the same method, but all different input
		//void add_acc(vector<ScoreReport> sr_list) {
		//		if (sr_list.size() == 0) {
		//				return add_acc(0);
		//		}
		//		double acc = 0, acc_fm=0;
		//		int match_count = 0;

		//		for each(ScoreReport sr in sr_list) {
		//				acc += sr.acc100;
		//				if (sr.firstWrong != 0) {
		//						match_count++;
		//				}
		//		}
		//		acc /= sr_list.size();
		//		_acc_of.push_back(acc);

		//}


		void report() {
				printf(" ===== ===== ===== ===== ===== \n");
				printf(" - minH:%i , hbins:%i , sbins:%i , hce_method_dbimg:%i \n", _minH, _hbins, _sbins, _hce_method_dbimg);

				if (other_setting.size() > 0)
						printf(" - other double setting: ");
				for (size_t i = 0; i < other_setting.size(); i++) {
						printf("#%i:%f , ", i, other_setting[i]);
				}
				if (other_setting.size() > 0)
						printf("\n");

				report_acc();
		}
		void report_acc() {
				for (int i = 0; i < _acc_of.size(); i++) {
						printf(" - method:%3i , overall acc: %f%% , first match count: %i , acc fm: %f%% \n", i + 1, _acc_of[i], _first_match_count[i], _acc_first_match[i]);
						double a_p = best_threshold_pr[i].first / (double)7.0;
						double a_r = best_threshold_pr[i].second / (double)7.0;
						printf("  - best_threshold: %f , p: %f%% , r: %f%% \n", best_threshold[i], a_p, a_r);
						if (_first_match_count[i] > 0) {
								double avg_p = best_threshold_fm_pr[i].first / (double) _first_match_count[i];
								double avg_r = best_threshold_fm_pr[i].second / (double) _first_match_count[i];
								printf("  - best_threshold_fm: %f , p: %f%% , r: %f%% \n", best_threshold_fm[i], avg_p, avg_r);
						}
						printf("\n");
				}
		}
};

struct BestMethodData {
		double bestGradeAP = 0, bestGradeAR = 0, score = 0;
		int method = 0; // method
		int bins[2];
		int minH = 0;
		bool a_method[METHOD_COUNT];
		int firstWrong;
		ScoreReport save_sr;
		vector<ScoreReport> srs;

		BestMethodData() { for (int i = 0; i < sizeof(a_method); i++) { a_method[i] = false; srs = vector<ScoreReport>(METHOD_COUNT); } }

		void update(ScoreReport sr, int _method, int hbins, int sbins, int minHe) {
				//sr.reportSorted(100);
				sr.report();
				BestMethodData bmd;
				srs[_method - 1] = sr;
				if (sr.bestGradeAindex != -1) {
						a_method[_method - 1] = true;
				}

				if (sr.bestGradeAindex != -1 && sr.bestGradeAPR() >(bestGradeAP * bestGradeAR)) {
						bestGradeAP = sr.bestGradeAP;
						bestGradeAR = sr.bestGradeAR;
						score = sr.score(sr.bestGradeAindex);
						save_sr = sr;
						method = _method;
						bins[0] = hbins;
						bins[1] = sbins;
						minH = minHe;
				}
				if (method <= 0 && sr.acc100 >= score) {
						score = sr.acc100;
						save_sr = sr;
						method = _method * -1;
						bins[0] = hbins;
						bins[1] = sbins;
						minH = minHe;
				}
		}
		void report() {
				cout << " - A method (1~n): ";
				for (int i = 0; i < 24; i++) { if (a_method[i])	cout << (i + 1) << ", "; }
				cout << endl;
				printf(" - Current best hbins: %i , sbins: %i , method: %i , minH: %i \n", bins[0], bins[1], method, minH);
				printf(" - Current best bestGradeAP: %.4f%% , GradeAR: %.4f%% , score: %.4f , firstWrong:%i  \n\n", bestGradeAP, bestGradeAR, score, save_sr.firstWrong);
		}
};