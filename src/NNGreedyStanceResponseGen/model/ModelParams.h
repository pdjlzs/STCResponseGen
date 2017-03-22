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

	Alphabet labelFeatAlpha; //should be intialized outside
	LookupTable labelFeats; // should be initialized outside

public:
	bool initial(HyperParams &opts, AlignedMemoryPool *mem) {

		opts.labelFeatDim = labelFeats.nDim;
		opts.word_dim = word_table.nDim;
		opts.word_represent_dim = opts.word_dim * 2;
		opts.word_window = 2 * opts.word_context + 1;
		opts.word_input2conv = opts.word_represent_dim * opts.word_window;
		opts.state_represent_dim = (opts.word_rnnhiddensize + opts.labelFeatDim) * 2 + opts.action_rnnhiddensize;


		word_conv.initial(opts.word_hiddensize, opts.word_input2conv, true, mem);
		word_left_lstm.initial(opts.word_rnnhiddensize, opts.word_hiddensize, mem); 
		word_right_lstm.initial(opts.word_rnnhiddensize, opts.word_hiddensize, mem);

		action_conv.initial(opts.action_hiddensize, opts.action_dim, opts.action_dim, true, mem);
		action_lstm.initial(opts.action_dim, opts.action_hiddensize, mem);
		scored_action_table.initial(&action_alpha, opts.state_hiddensize, true);
		state_hidden.initial(opts.state_hiddensize, opts.state_represent_dim, true, mem);

		return true;
	}

	bool initial(HyperParams &opts){

		opts.labelFeatDim = labelFeats.nDim;
		opts.word_dim = word_table.nDim;
		opts.word_represent_dim = opts.word_dim * 2;
		opts.word_window = 2 * opts.word_context + 1;
		opts.word_input2conv = opts.word_represent_dim * opts.word_window;
		opts.state_represent_dim = (opts.word_rnnhiddensize + opts.labelFeatDim) * 2 + opts.action_rnnhiddensize;
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

		labelFeats.exportAdaParams(ada);
	}

	// will add it later
	void saveModel(std::ofstream &os) {
		word_alpha.write(os);
		word_table.save(os);
		word_ext_alphas.write(os);
		word_ext_table.save(os);
		labelFeatAlpha.write(os);
		labelFeats.save(os);


		word_conv.save(os);
		word_left_lstm.save(os);
		word_right_lstm.save(os);

		action_alpha.write(os);
		action_table.save(os);
		action_conv.save(os);
		action_lstm.save(os);

		scored_action_table.save(os);

		state_hidden.save(os);
	}

	void loadModel(std::ifstream &is, AlignedMemoryPool* mem = NULL) {
		word_alpha.read(is);
		word_table.load(is, &word_alpha, mem);
		word_ext_alphas.read(is);
		word_ext_table.load(is, &word_ext_alphas, mem);
		labelFeatAlpha.read(is);
		labelFeats.load(is, &labelFeatAlpha, mem);

		word_conv.load(is, mem);
		word_left_lstm.load(is, mem);
		word_right_lstm.load(is, mem);

		action_alpha.read(is);
		action_table.load(is, &action_alpha, mem);
		action_conv.load(is, mem);
		action_lstm.load(is, mem);

		scored_action_table.load(is, &action_alpha, mem);

		state_hidden.load(is, mem);
	}

	void loadPretrainModel(std::ifstream &is, HyperParams &opts, AlignedMemoryPool* mem = NULL) {

		opts.labelFeatDim = labelFeats.nDim;
		opts.word_represent_dim = opts.word_dim * 2;
		opts.word_window = 2 * opts.word_context + 1;
		opts.word_input2conv = opts.word_represent_dim * opts.word_window;
		opts.state_represent_dim = (opts.word_rnnhiddensize + opts.labelFeatDim) * 2 + opts.action_rnnhiddensize;

		word_conv.initial(opts.word_hiddensize, opts.word_input2conv, true, mem);
		word_left_lstm.initial(opts.word_rnnhiddensize, opts.word_hiddensize, mem);
		word_right_lstm.initial(opts.word_rnnhiddensize, opts.word_hiddensize, mem);

		action_conv.initial(opts.action_hiddensize, opts.action_dim, opts.action_dim, true, mem);
		action_lstm.initial(opts.action_dim, opts.action_hiddensize, mem);
		state_hidden.initial(opts.state_hiddensize, opts.state_represent_dim, true, mem);

		word_alpha.read(is);
		word_table.initial(&word_alpha, opts.word_dim, true);
		word_table.load(is, &word_alpha, mem);
		word_ext_alphas.read(is);
		word_ext_table.initial(&word_ext_alphas, opts.word_dim, false);
		word_ext_table.load(is, &word_ext_alphas, mem);

		word_conv.load(is, mem);
		word_left_lstm.load(is, mem);
		word_right_lstm.load(is, mem);

		action_alpha.read(is);
		action_table.initial(&action_alpha, opts.action_dim, true);
		action_table.load(is, &action_alpha, mem);
		action_conv.load(is, mem);
		action_lstm.load(is, mem);

		scored_action_table.initial(&action_alpha, opts.state_hiddensize, true);
		scored_action_table.load(is, &action_alpha, mem);

		state_hidden.load(is, mem);
	}

};

#endif /* SRC_ModelParams_H_ */
