#ifndef SRC_GlobalNodes_H_
#define SRC_GlobalNodes_H_

#include "ModelParams.h"

struct GlobalNodes {
	vector<LookupNode> word_inputs;
	WindowBuilder word_window;
	vector<UniNode> window_tanh_conv;
	vector<IncLSTM1Builder> words_lstm;
	IncLSTM1Builder* word_lstm;
	//LSTM1Builder word_left_lstm;
	//LSTM1Builder word_right_lstm;

public:
	inline void resize(int max_length) {
		word_inputs.resize(max_length);
		word_window.resize(max_length);
		window_tanh_conv.resize(max_length);
		words_lstm.resize(max_length);
		//word_left_lstm.resize(max_length);
		//word_right_lstm.resize(max_length);
	}

public:
	inline void initial(ModelParams &params, HyperParams &hyparams, AlignedMemoryPool *mem) {
		int length = word_inputs.size();
		for (int idx = 0; idx < length; idx++) {
			word_inputs[idx].setParam(&params.word_table);
			word_inputs[idx].init(hyparams.word_dim, hyparams.dropProb, mem);
			window_tanh_conv[idx].setParam(&params.word_conv);
			window_tanh_conv[idx].init(hyparams.word_dim, hyparams.word_context, mem);
			words_lstm[idx].init(&params.word_left_lstm, hyparams.dropProb, mem);
		}

		//word_left_lstm.init(&params.word_left_lstm, hyparams.dropProb, true, mem);
		//word_right_lstm.init(&params.word_right_lstm, hyparams.dropProb, false, mem);
	}


public:
	inline void forward(Graph* cg, const vector<string>* words, HyperParams* hyparams){
		int word_size = words->size();
		string currWord, currPos;
		for (int idx = 0; idx < word_size; idx++){
			currWord = (*words)[idx];
			word_inputs[idx].forward(cg, currWord);
		}
		word_window.forward(cg, getPNodes(word_inputs, word_size));

		for (int idx = 0; idx < word_size; idx++){
			window_tanh_conv[idx].forward(cg, &word_window._outputs[idx]);
			if (idx == 0)
				words_lstm[idx].forward(cg, &(window_tanh_conv[idx]), NULL);
			else
				words_lstm[idx].forward(cg, &(window_tanh_conv[idx]), &words_lstm[idx - 1]);
		}
		word_lstm = &words_lstm[word_size - 1];

		//word_left_lstm.forward(cg, getPNodes(window_tanh_conv, word_size));
		//word_right_lstm.forward(cg, getPNodes(window_tanh_conv, word_size));

	}

};

#endif SRC_GlobalNodes_H_
