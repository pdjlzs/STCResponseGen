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


	void evaluate(const vector<string>& resulted_respon, Metric& eval) const {
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

