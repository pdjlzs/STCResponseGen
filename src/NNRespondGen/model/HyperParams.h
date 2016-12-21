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
	unordered_map<string, vector<string> > reverse_word_map;//for actions


	dtype nnRegular; // for optimization
	dtype adaAlpha;  // for optimization
	dtype adaEps; // for optimization
	dtype dropProb;

	int char_dim; 
	int chartype_dim;
	int bichar_dim;
	int word_dim;
	int action_dim;

	bool char_tune;
	bool bichar_tune;
	bool word_tune;

	int char_context;
	int char_repsentation_dim;
	int char_window_dim;

	int char_hidden_dim;  
	int word_hidden_dim;
	int action_hidden_dim;

	int char_lstm_dim;
	int word_lstm_dim;
	int action_lstm_dim;

	int sep_hidden_dim;
	int app_hidden_dim;

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

		char_dim = opt.charEmbSize;
		bichar_dim = opt.bicharEmbSize;
		chartype_dim = opt.charTypeEmbSize;
		word_dim = opt.wordEmbSize;
		action_dim = opt.actionEmbSize;

		char_tune = opt.charEmbFineTune;
		bichar_tune = opt.bicharEmbFineTune;
		word_tune = opt.wordEmbFineTune;

		char_context = opt.charcontext;
		char_repsentation_dim = char_dim + bichar_dim + chartype_dim;
		char_window_dim = (2 * char_context + 1) * char_repsentation_dim;

		char_hidden_dim = opt.charHiddenSize;
		word_hidden_dim = opt.wordHiddenSize;
		action_hidden_dim = opt.actionHiddenSize;

		char_lstm_dim = opt.charRNNHiddenSize;
		word_lstm_dim = opt.wordRNNHiddenSize;
		action_lstm_dim = opt.actionRNNHiddenSize;

		sep_hidden_dim = opt.sepHiddenSize;
		app_hidden_dim = opt.appHiddenSize;
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