#ifndef SRC_ModelParams_H_
#define SRC_ModelParams_H_

#include "HyperParams.h"

// Each model consists of two parts, building neural graph and defining output losses.
class ModelParams {

public:
	//neural parameters
	//parameters used in global node
	Alphabet word_alpha; // words
	LookupTable word_table; // should be initialized outside
	Alphabet word_ext_alphas; // pre-train/external word embeddings
	LookupTable word_ext_table; // should be initialized outside

	UniParams word_conv; // word tanh after concat
	LSTM1Params word_left_lstm; //left lstm
	LSTM1Params word_right_lstm; //right lstm

	//parameters used in action nodes
	Alphabet action_alpha;
	LookupTable action_table; // should be initialized outside
	BiParams action_conv;
	LSTM1Params action_lstm;

	LookupTable scored_action_table;

	UniParams state_hidden;
public:
	bool initial(HyperParams &opts, AlignedMemoryPool *mem) {

		opts.word_dim = word_table.nDim;
		opts.word_represent_dim = opts.word_dim * 2;
		opts.word_window = 2 * opts.word_context + 1;
		opts.word_input2conv = opts.word_represent_dim * opts.word_window;
		opts.state_represent_dim = opts.word_rnnhiddensize * 2 + opts.action_rnnhiddensize;


		word_conv.initial(opts.word_hiddensize, opts.word_input2conv, true, mem);
		word_left_lstm.initial(opts.word_rnnhiddensize, opts.word_hiddensize, mem); 
		word_right_lstm.initial(opts.word_rnnhiddensize, opts.word_hiddensize, mem);

		action_conv.initial(opts.action_hiddensize, opts.action_dim, opts.action_dim, true, mem);
		action_lstm.initial(opts.action_dim, opts.action_hiddensize, mem);
		scored_action_table.initial(&action_alpha, opts.state_hiddensize, true);
		state_hidden.initial(opts.state_hiddensize, opts.state_represent_dim, true, mem);

		return true;
	}


	void exportModelParams(ModelUpdate &ada) {
		//neural features
		word_table.exportAdaParams(ada);
		word_ext_table.exportAdaParams(ada);
		word_conv.exportAdaParams(ada);
		word_left_lstm.exportAdaParams(ada);
		word_right_lstm.exportAdaParams(ada);
		action_table.exportAdaParams(ada);

		action_conv.exportAdaParams(ada);
		action_lstm.exportAdaParams(ada);
		state_hidden.exportAdaParams(ada);
		scored_action_table.exportAdaParams(ada);
	}

	// will add it later
	void saveModel() {

	}

	void loadModel(const string &inFile) {

	}

};

#endif /* SRC_ModelParams_H_ */
