#ifndef SRC_HyperParams_H_
#define SRC_HyperParams_H_

#include "N3L.h"
#include "Options.h"

using namespace nr;
using namespace std;

struct HyperParams{
	//required
	int beam;
	int maxlength;
	int action_num;
	dtype delta;
	unordered_map<string, vector<string> > word_map;//for actions


	dtype nnRegular; // for optimization
	dtype adaAlpha;  // for optimization
	dtype adaEps; // for optimization
	dtype dropProb;

	bool word_tune;
	int word_dim;
	int word_context;
	int word_hidden_dim;
	int word_lstm_dim;

	int action_dim;

	int action_hidden_dim;

	int action_lstm_dim;

	int sep_hidden_dim;

public:
	HyperParams(){
		maxlength = max_sentence_clength + 1;
		bAssigned = false;
	}

public:
	void setRequared(Options& opt){
		//please specify dictionary outside
		//please sepcify char_dim, word_dim and action_dim outside.
		beam = opt.beam;
		delta = opt.delta;
		bAssigned = true;

		nnRegular = opt.regParameter;
		adaAlpha = opt.adaAlpha;
		adaEps = opt.adaEps;
		dropProb = opt.dropProb;

		word_tune = opt.wordEmbFineTune;
		word_dim = opt.wordEmbSize;
		word_context = opt.wordcontext;
		word_hidden_dim = opt.wordHiddenSize;
		word_lstm_dim = opt.wordRNNHiddenSize;
		action_dim = opt.actionEmbSize;

		action_hidden_dim = opt.actionHiddenSize;

		action_lstm_dim = opt.actionRNNHiddenSize;

		sep_hidden_dim = opt.sepHiddenSize;
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