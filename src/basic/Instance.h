#ifndef _JST_INSTANCE_
#define _JST_INSTANCE_

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "N3L.h"
#include "Metric.h"
#include "HyperParams.h"
#include "Evaluation.h"

using namespace std;

class Instance {
public:
	Instance() {
	}
	~Instance() {
	}

	int postWordsize() const {
		return post_words.size();
	}

	int responWordsize() const{
		return respon_words.size();
	}

	int responCharsize() const {
		return respon_chars.size();
	}

	void clear() {
		post_words.clear();
		respon_words.clear();
		respon_chars.clear();
	}

	void allocate(int postlength, int responlength, int charLength) {
		clear();
		post_words.resize(postlength);
		respon_words.resize(responlength);
		respon_chars.resize(charLength);
	}

	void copyValuesFrom(const Instance& anInstance) {
		allocate(anInstance.postWordsize(), anInstance.responWordsize(), anInstance.responCharsize());
		for (int i = 0; i < anInstance.postWordsize(); i++) {
			post_words[i] = anInstance.post_words[i];
		}
		for (int i = 0; i < anInstance.responWordsize(); i++){
			respon_words[i] = anInstance.respon_words[i];
		}
		for (int i = 0; i < anInstance.responCharsize(); i++) {
			respon_chars[i] = anInstance.respon_chars[i];
		}
	}

	double getUnigram(const string& cur_word, HyperParams& opts) const{

		unordered_map<string, float >& uni_pro = opts.uni_pro;

		if (uni_pro.find(cur_word) != uni_pro.end()){
			return uni_pro[cur_word];
		}
		else return uni_pro["-null-"];

	}

	double getBigram(const string& last_word, const string& cur_word, HyperParams& opts) const {

		unordered_map<string, float >& uni_back = opts.uni_back;
		unordered_map<string, float >& bi_pro = opts.bi_pro;

		string bigram = last_word + "#" + cur_word;

		if (bi_pro.find(bigram) != bi_pro.end()) {  //hit bi-gram
			return bi_pro[bigram];
		}
		else if (uni_back.find(last_word) != uni_back.end())   // bi-gram = backoff(last_word) + pro(cur_word)
			return uni_back[last_word] + getUnigram(cur_word, opts);
		else return -7;
	}

	double getTrigram(const string& last2_word, const string& last_word, const string& cur_word, HyperParams& opts) const {

		unordered_map<string, float >& bi_back = opts.bi_back;
		unordered_map<string, float >& tri_pro = opts.tri_pro;

		string bigram = last_word + "#" + cur_word;
		string trigram = last2_word + "#" + last_word + "#" + cur_word;
		string backoffgram = last2_word + "#" + last_word;

		if (tri_pro.find(trigram) != tri_pro.end()) {  //hit tri-gram
			return tri_pro[trigram];
		}
		else if (bi_back.find(backoffgram) != bi_back.end() ) { // tri-gram = backoff(last2_word + last_word) + pro(bigram)
			return bi_back[backoffgram] + getBigram(last_word, cur_word, opts);
		}
		else {	//tri-gram = 0.7 * pro(trigram) + 0.2 * pro(bigram) + 0.1 * pro(unigram)
			return  0.7*-7 + 0.2*getBigram(last_word, cur_word, opts) + 0.1* getUnigram(cur_word, opts);
		}

	}

	void evaluate(const vector<string>& resulted_respon, Evaluation& eval, HyperParams& opts) const {

		vector<string> sent;
		for (int index = 0; index < resulted_respon.size(); index++)
			sent.push_back(resulted_respon[index]);
		sent.push_back("-end-");


		double sent_probabli = 0;
		int len = sent.size();
		for (int i = 0; i < len; i++){
			double gram_prob = 0;
			string last2_word = i - 2 < 0 ? "-start-" : sent[i - 2];
			string last_word = i - 1 < 0 ? "-start-" : sent[i - 1];
			string cur_word = sent[i];

			gram_prob = getTrigram(last2_word, last_word, cur_word, opts);

			sent_probabli += gram_prob;
		}


		eval.sum_logprobabi += sent_probabli;
		eval.word_num += sent.size();

	}

	void bIdentical(const vector<string>& resulted_respon, Metric& eval) const {
		unordered_set<string> golds;
		getSegIndexes(respon_words, golds);
		unordered_set<string> preds;
		getSegIndexes(resulted_respon, preds);
		unordered_set<string>::iterator iter;
		eval.overall_label_count += golds.size();
		eval.predicated_label_count += preds.size();
		for (iter = preds.begin(); iter != preds.end(); iter++) {
			if (golds.find(*iter) != golds.end()) {
				eval.correct_label_count++;
			}
		}
	}

	void getSegIndexes(const vector<string>& respon, unordered_set<string>& responIndexes) const {
		responIndexes.clear();
		int idx = 0, idy = 0;
		string curWord = "";
		int beginId = 0;
		while (idx < respon_chars.size() && idy < respon.size()) {
			curWord = curWord + respon_chars[idx];
			if (curWord.length() == respon[idy].length()) {
				stringstream ss;
				ss << "[" << beginId << "," << idx << "]";
				responIndexes.insert(ss.str());
				idy++;
				beginId = idx + 1;
				curWord = "";
			}
			idx++;
		}

		/*if (idx != respon_chars.size() || idy != segs.size()) {
			std::cout << "error segs, please check" << std::endl;
			}*/
	}

	/*
	void getNormSegIndexes(const vector<string>& segs, const vector<string>& normsegs, unordered_set<string>& segIndexes) const {
	segIndexes.clear();
	int idx = 0, idy = 0;
	string curWord = "";
	int beginId = 0;
	while (idx < respon_chars.size() && idy < segs.size()) {
	curWord = curWord + respon_chars[idx];
	if (curWord.length() == segs[idy].length()) {
	stringstream ss;
	ss << normsegs[idy] + "[" << beginId << "," << idx << "]";
	segIndexes.insert(ss.str());
	idy++;
	beginId = idx + 1;
	curWord = "";
	}
	idx++;
	}

	if (idx != respon_chars.size() || idy != segs.size()) {
	std::cout << "error segs, please check" << std::endl;
	}
	}
	*/

public:
	vector<string> post_words;
	vector<string> respon_words;
	vector<string> respon_chars;
};

#endif

