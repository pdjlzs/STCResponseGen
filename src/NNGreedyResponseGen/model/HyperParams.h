#ifndef SRC_HyperParams_H_
#define SRC_HyperParams_H_

#include "N3L.h"
#include "Options.h"

using namespace nr;
using namespace std;

struct HyperParams{
	//required
	int maxlength;
	int action_num;
	int maxCandidAction;
	dtype delta;
	unordered_map<string, vector<string> > trigram_candid;	// tri-gram dic for get actions candidate
	unordered_map<string, vector<int> > random_tabel;		// each word have more than maxCandidAction wil have a random_tabel
	unordered_map<string, int> triword_stat;	// tri-gram word freq for random tabel
	unordered_map<string, int> word_stat;


	unordered_map<string, float > uni_pro;	// ui-gram dic for calculate perplexity
	unordered_map<string, float > uni_back;	// ui-gram dic for calculate perplexity
	unordered_map<string, float > bi_pro;	// bi-gram dic for calculate perplexity
	unordered_map<string, float > bi_back;	// bi-gram dic for calculate perplexity
	unordered_map<string, float > tri_pro;	// tri-gram dic calculate perplexity


	dtype nnRegular; // for optimization
	dtype adaAlpha;  // for optimization
	dtype adaEps; // for optimization
	dtype dropProb;

	//must assign
	int word_context;
	int word_hiddensize;
	int word_rnnhiddensize;

	int action_dim;
	int action_hiddensize;
	int action_rnnhiddensize;

	int state_hiddensize;

	int unk_strategy;

	//auto generare
	int word_dim;
	int word_represent_dim;
	int word_window;
	int word_input2conv;
	int state_represent_dim;
public:
	HyperParams(){
		//maxlength = max_sentence_clength + 1;
		bAssigned = false;
	}

public:
	void setRequared(Options& opt, int noActionCode){
		//please specify dictionary outside
		//please sepcify char_dim, word_dim and action_dim outside.
		maxlength = opt.maxLength;
		action_num = noActionCode + opt.maxCandidAction;
		maxCandidAction = opt.maxCandidAction;
		delta = opt.delta;
		bAssigned = true;

		nnRegular = opt.regParameter;
		adaAlpha = opt.adaAlpha;
		adaEps = opt.adaEps;
		dropProb = opt.dropProb;

		word_context = opt.wordcontext;
		word_hiddensize = opt.wordHiddenSize;
		word_rnnhiddensize = opt.wordRNNHiddenSize;

		action_dim = opt.actionEmbSize;
		action_hiddensize = opt.actionHiddenSize;
		action_rnnhiddensize = opt.actionRNNHiddenSize;

		state_hiddensize = opt.state_hiddensize;

		unk_strategy = opt.unkStrategy;

	}

	void loadModel(std::ifstream &is) {
		is >> maxlength;
		is >> action_num;
		is >> maxCandidAction;
		is >> delta;
		is >> bAssigned;
		is >> nnRegular;
		is >> adaAlpha;
		is >> adaEps;
		is >> dropProb;
		is >> word_context;
		is >> word_hiddensize;
		is >> word_rnnhiddensize;
		is >> action_dim;
		is >> action_hiddensize;
		is >> action_rnnhiddensize;
		is >> state_hiddensize;
		is >> unk_strategy;
		is >> word_dim;

		int size = 0;
		is >> size;
		for (int i = 0; i < size; i++){
			string bigram_word;
			is >> bigram_word;
			int tempsize;
			is >> tempsize;
			for (int j = 0; j < tempsize; j++){
				string cur_word;
				is >> cur_word;
				trigram_candid[bigram_word].push_back(cur_word);
			}
		}

		is >> size;
		for (int i = 0; i < size; i++){
			string cur_word;
			is >> cur_word;
			int freq;
			is >> freq;
			triword_stat[cur_word] = freq;
		}

		int table_size;
		const float power = 0.75;
		unordered_map<string, vector<string> >::iterator it;
		for (it = trigram_candid.begin(); it != trigram_candid.end(); it++){
			if (it->second.size() > maxCandidAction){
				if (it->first == "-start-#-start-") table_size = 1e6;
				else if (it->second.size() >= 900) table_size = 1e5;
				else table_size = 1e4;
				random_tabel[it->first].resize(table_size);
				double d1 = 0, train_words_pow = 0;
				vector<int> vocab;
				for (int i = 0; i < it->second.size(); i++){
					string trigram_words = it->first + "#" + it->second[i];
					int trigram_freq = triword_stat[trigram_words];
					train_words_pow += pow(trigram_freq, power);
					vocab.push_back(trigram_freq);
				}
				int i = 0;
				d1 = pow(vocab[i], power) / train_words_pow;
				for (int a = 0; a < table_size; a++) {
					random_tabel[it->first][a] = i;
					if (a / (double)table_size > d1) {
						i++;
						d1 += pow(vocab[i], power) / train_words_pow;
					}
					if (i >= vocab.size()) i = vocab.size() - 1;
				}
				int last_word_candi_index = random_tabel[it->first][table_size - 1];
				if (last_word_candi_index != it->second.size() - 1)
					cout << "Error! Not all candidate are in the random tabel" << last_word_candi_index << "!=" << it->second.size() - 1 << endl;
			}
		}


		is >> size;
		for (int i = 0; i < size; i++){
			string cur_word;
			is >> cur_word;
			int freq;
			is >> freq;
			word_stat[cur_word] = freq;
		}

	}

	void saveModel(std::ofstream &os) {
		os << maxlength << endl;
		os << action_num << endl;
		os << maxCandidAction << endl;
		os << delta << endl;
		os << bAssigned << endl;
		os << nnRegular << endl;
		os << adaAlpha << endl;
		os << adaEps << endl;
		os << dropProb << endl;
		os << word_context << endl;
		os << word_hiddensize << endl;
		os << word_rnnhiddensize << endl;
		os << action_dim << endl;
		os << action_hiddensize << endl;
		os << action_rnnhiddensize << endl;
		os << state_hiddensize << endl;
		os << unk_strategy << endl;
		os << word_dim << endl;

		os << trigram_candid.size() << endl;
		unordered_map<string, vector<string> >::iterator umit = trigram_candid.begin();
		for (; umit != trigram_candid.end(); umit++){
			os << umit->first << " ";
			vector<string> &tempvec = umit->second;
			os << tempvec.size() << " ";
			os << tempvec[0];
			for (int i = 1; i < tempvec.size(); i++){
				os << " " << tempvec[i];
			}
			os << endl;
		}

		os << triword_stat.size() << endl;
		unordered_map<string, int> ::iterator umit3 = triword_stat.begin();
		for (; umit3 != triword_stat.end(); umit3++){
			os << umit3->first << " " << umit3->second << endl;
		}

		os << word_stat.size() << endl;
		unordered_map<string, int> ::iterator umit2 = word_stat.begin();
		for (; umit2 != word_stat.end(); umit2++){
			os << umit2->first << " " << umit2->second << endl;
		}

	}
	void clear(){
		bAssigned = false;
	}

	bool bValid(){
		return bAssigned;
	}


public:

	void print(){
		cout << "maxlength= " << maxlength << endl;
		cout << "action_num= " << action_num << endl;
		cout << "maxCandidAction= " << maxCandidAction << endl;
		cout << "delta= " << delta << endl;
		cout << "bAssigned= " << bAssigned << endl;
		cout << "nnRegular= " << nnRegular << endl;
		cout << "adaAlpha= " << adaAlpha << endl;
		cout << "adaEps= " << adaEps << endl;
		cout << "dropProb= " << dropProb << endl;
		cout << "word_context= " << word_context << endl;
		cout << "word_hiddensize= " << word_hiddensize << endl;
		cout << "word_rnnhiddensize= " << word_rnnhiddensize << endl;
		cout << "action_dim= " << action_dim << endl;
		cout << "action_hiddensize= " << action_hiddensize << endl;
		cout << "action_rnnhiddensize= " << action_rnnhiddensize << endl;
		cout << "state_hiddensize= " << state_hiddensize << endl;
		cout << "unk_strategy= " << unk_strategy << endl;
		cout << "word_dim=" << word_dim << endl;
	}

private:
	bool bAssigned;
};


#endif /* SRC_HyperParams_H_ */