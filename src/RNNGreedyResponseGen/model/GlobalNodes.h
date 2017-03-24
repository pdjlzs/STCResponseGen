#ifndef SRC_GlobalNodes_H_
#define SRC_GlobalNodes_H_

#include "ModelParams.h"

struct GlobalNodes {
	vector<LookupNode> word_inputs;
	vector<LookupNode> word_ext_inputs;
	vector<ConcatNode> word_represent;
	WindowBuilder word_window;
	vector<UniNode> window_tanh_conv;
	LSTM1Builder words_left_lstm;
	LSTM1Builder words_right_lstm;
	PNode word_left_lstm;
	PNode word_right_lstm;

public:
	inline void resize(int max_length) {
		word_inputs.resize(max_length);
		word_ext_inputs.resize(max_length);
		word_represent.resize(max_length);
		word_window.resize(max_length);
		window_tanh_conv.resize(max_length);
		words_left_lstm.resize(max_length);
		words_right_lstm.resize(max_length);
	}

public:
	inline void initial(ModelParams &params, HyperParams &opts, AlignedMemoryPool *mem) {
		int word_dim = params.word_table.nDim;
		int length = word_inputs.size();
		for (int idx = 0; idx < length; idx++) {
			word_inputs[idx].setParam(&params.word_table);
			word_inputs[idx].init(word_dim, opts.dropProb, mem);
			word_ext_inputs[idx].setParam(&params.word_ext_table);
			word_ext_inputs[idx].init(word_dim, opts.dropProb, mem);
			word_represent[idx].init(opts.word_represent_dim, -1, mem);
			window_tanh_conv[idx].setParam(&params.word_conv);
			window_tanh_conv[idx].init(opts.word_hiddensize, opts.dropProb, mem);
		}

		word_window.init(opts.word_represent_dim, opts.word_context, mem);
		words_left_lstm.init(&params.word_left_lstm, opts.dropProb, true, mem);
		words_right_lstm.init(&params.word_right_lstm, opts.dropProb, false, mem);
	}


public:
	inline void forward(Graph* cg, const vector<string>& words, HyperParams* hyparams){
		string currWord;
		int word_size = words.size();
		for (int idx = 0; idx < word_size; idx++){
			currWord = words[idx];
			word_ext_inputs[idx].forward(cg, currWord);
			int c;
			if (hyparams->word_stat.find(currWord) != hyparams->word_stat.end())
				c = hyparams->word_stat[currWord];
			else c = 0;
			bool  dropFlag = cg->train && (rand() / double(RAND_MAX) >= (c / (0.25 + c)));
			if (dropFlag && hyparams->unk_strategy == 1) {
				currWord = unknownkey;
			}
			word_inputs[idx].forward(cg, currWord);
			word_represent[idx].forward(cg, &word_inputs[idx], &word_ext_inputs[idx]);
		}


		word_window.forward(cg, getPNodes(word_represent, word_size));

		for (int idx = 0; idx < word_size; idx++){
			window_tanh_conv[idx].forward(cg, &word_window._outputs[idx]);
		}

		words_left_lstm.forward(cg, getPNodes(window_tanh_conv, word_size));
		words_right_lstm.forward(cg, getPNodes(window_tanh_conv, word_size));

		word_left_lstm = &words_left_lstm._hiddens[word_size - 1];
		word_right_lstm = &words_right_lstm._hiddens[0];
	}

};

#endif SRC_GlobalNodes_H_
