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

		unk_strategy = 1;
	}

	void clear(){
		bAssigned = false;
	}

	bool bValid(){
		return bAssigned;
	}


public:

	void print(){

	}

private:
	bool bAssigned;
};


#endif /* SRC_HyperParams_H_ */