#ifndef _JST_INSTANCE_
#define _JST_INSTANCE_

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "N3L.h"
#include "Metric.h"

using namespace std;

class Instance {
public:
	Instance() {
	}
	~Instance() {
	}

	int wordsize() const {
		return words.size();
	}

	int respon_wordsize() const{
		return normwords.size();
	}

	int charsize() const {
		return chars.size();
	}

	void clear() {
		words.clear();
		chars.clear();
		normwords.clear();
	}

	void allocate(int postlength, int responlength, int charLength) {
		clear();
		words.resize(postlength);
		normwords.resize(responlength);
		chars.resize(charLength);
	}

	void copyValuesFrom(const Instance& anInstance) {
		allocate(anInstance.wordsize(), anInstance.respon_wordsize(), anInstance.charsize());
		for (int i = 0; i < anInstance.wordsize(); i++) {
			words[i] = anInstance.words[i];
		}
		for (int i = 0; i < anInstance.respon_wordsize(); i++){
			normwords[i] = anInstance.normwords[i];
		}
		for (int i = 0; i < anInstance.charsize(); i++) {
			chars[i] = anInstance.chars[i];
		}
	}


	void evaluate(const vector<string>& resulted_segs, const vector<string>& resulted_normsegs, Metric& eval, Metric& normeval) const {
		unordered_set<string> golds;
		getSegIndexes(normwords, golds);

		unordered_set<string> preds;
		getSegIndexes(resulted_normsegs, preds);

		unordered_set<string>::iterator iter;
		eval.overall_label_count += golds.size();
		eval.predicated_label_count += preds.size();
		for (iter = preds.begin(); iter != preds.end(); iter++) {
			if (golds.find(*iter) != golds.end()) {
				eval.correct_label_count++;
			}
		}

		/*
		unordered_set<string> goldnorms;
		getNormSegIndexes(words, normwords, goldnorms);

		unordered_set<string> prednorms;
		getNormSegIndexes(resulted_segs, resulted_normsegs, prednorms);

		normeval.overall_label_count += goldnorms.size();
		normeval.predicated_label_count += prednorms.size();
		for (iter = prednorms.begin(); iter != prednorms.end(); iter++) {
			if (goldnorms.find(*iter) != goldnorms.end()) {
				normeval.correct_label_count++;
			}
		}*/

	}

	void getSegIndexes(const vector<string>& segs, unordered_set<string>& segIndexes) const {
		segIndexes.clear();
		int idx = 0, idy = 0;
		string curWord = "";
		int beginId = 0;
		while (idx < chars.size() && idy < segs.size()) {
			curWord = curWord + chars[idx];
			if (curWord.length() == segs[idy].length()) {
				stringstream ss;
				ss << "[" << beginId << "," << idx << "]";
				segIndexes.insert(ss.str());
				idy++;
				beginId = idx + 1;
				curWord = "";
			}
			idx++;
		}

		/*if (idx != chars.size() || idy != segs.size()) {
			std::cout << "error segs, please check" << std::endl;
		}*/
	}

	void getNormSegIndexes(const vector<string>& segs, const vector<string>& normsegs, unordered_set<string>& segIndexes) const {
		segIndexes.clear();
		int idx = 0, idy = 0;
		string curWord = "";
		int beginId = 0;
		while (idx < chars.size() && idy < segs.size()) {
			curWord = curWord + chars[idx];
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

		if (idx != chars.size() || idy != segs.size()) {
			std::cout << "error segs, please check" << std::endl;
		}
	}

public:
	vector<string> words;
	vector<string> normwords;
	vector<string> chars;
};

#endif

