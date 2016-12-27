#ifndef SRC_ModelParams_H_
#define SRC_ModelParams_H_

#include "HyperParams.h"

// Each model consists of two parts, building neural graph and defining output losses.
class ModelParams {

public:
	//neural parameters
	//parameters used in global node
	Alphabet embeded_words; // words
	LookupTable word_table; // should be initialized outside
	BiParams word_conv;
	LSTM1Params word_left_lstm; //left lstm
	//LSTM1Params word_right_lstm; //right lstm

	Alphabet embeded_actions;
	LookupTable action_table; // should be initialized outside
	BiParams action_conv;
	LSTM1Params action_lstm;

	BiParams state_hidden;
	UniParams score;
public:
	bool initial(HyperParams &opts, AlignedMemoryPool *mem) {
		int word_represent_dim = opts.word_dim * opts.word_context;

		word_conv.initial(opts.word_hidden_dim, word_represent_dim, true, mem);
		word_left_lstm.initial(opts.word_lstm_dim, opts.word_hidden_dim, mem); //left lstm
		//word_right_lstm.initial(opts.word_lstm_dim, opts.word_hidden_dim, mem); //right lstm
		/*
		action_lstm.initial(opts.action_lstm_dim, opts.action_hidden_dim, mem);
		state_hidden.initial(opts.state_hidden_dim, opts.state_concat_dim, true, mem);
		scored_action_table.initial(&embeded_actions, opts.state_hidden_dim, true);
		bi_word_conv.initial(opts.bi_word_hidden_dim, opts.bi_word_represention_dim, true, mem);

		bi_word_left_lstm.initial(opts.bi_word_lstm_dim, opts.bi_word_hidden_dim, mem); //left lstm
		bi_word_right_lstm.initial(opts.bi_word_lstm_dim, opts.bi_word_hidden_dim, mem); //right lstm
		*/

		return true;
	}


	void exportModelParams(ModelUpdate &ada) {
		//neural features
		word_table.exportAdaParams(ada);
		word_conv.exportAdaParams(ada);
		word_left_lstm.exportAdaParams(ada);
		//word_right_lstm.exportAdaParams(ada);
		/*
		action_table.exportAdaParams(ada);

		action_conv.exportAdaParams(ada);
		action_lstm.exportAdaParams(ada);
		state_hidden.exportAdaParams(ada);
		scored_action_table.exportAdaParams(ada);
		bi_words.exportAdaParams(ada);
		bi_word_left_lstm.exportAdaParams(ada);
		bi_word_right_lstm.exportAdaParams(ada);
		bi_word_conv.exportAdaParams(ada);
		*/
	}

	// will add it later
	void saveModel() {

	}

	void loadModel(const string &inFile) {

	}

};

#endif /* SRC_ModelParams_H_ */
