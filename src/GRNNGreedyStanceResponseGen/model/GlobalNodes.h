#ifndef SRC_GlobalNodes_H_
#define SRC_GlobalNodes_H_

#include "ModelParams.h"
#include "Instance.h"

struct GlobalNodes {
	vector<LookupNode> word_inputs;
	vector<LookupNode> word_ext_inputs;
	vector<ConcatNode> word_represent;
	WindowBuilder word_window;
	vector<UniNode> window_tanh_conv;
	GRNNBuilder  words_left_grnn;
	GRNNBuilder  words_right_grnn;
	PNode word_left_lstm;
	PNode word_right_lstm;
	LookupNode stance_label;
	ConcatNode left_concat_labelFeat;
	ConcatNode right_concat_labelFeat;

public:
	inline void resize(int max_length) {
		word_inputs.resize(max_length);
		word_ext_inputs.resize(max_length);
		word_represent.resize(max_length);
		word_window.resize(max_length);
		window_tanh_conv.resize(max_length);
		words_left_grnn.resize(max_length);
		words_right_grnn.resize(max_length);
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
		words_left_grnn.init(&params.word_left_grnn, opts.dropProb, true, mem);
		words_right_grnn.init(&params.word_right_grnn, opts.dropProb, false, mem);

		stance_label.init(params.label_table.nDim, opts.dropProb, mem);
		stance_label.setParam(&params.label_table);
		left_concat_labelFeat.init(opts.word_rnnhiddensize + params.label_table.nDim, -1, mem);
		right_concat_labelFeat.init(opts.word_rnnhiddensize + params.label_table.nDim, -1, mem);
	}


public:
	inline void forward(Graph* cg, const Instance& inst, HyperParams* hyparams){
		const vector<string>& words = inst.post_words;
		string stance = inst.stance_label;
		stance_label.forward(cg, stance);


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

		words_left_grnn.forward(cg, getPNodes(window_tanh_conv, word_size));
		words_right_grnn.forward(cg, getPNodes(window_tanh_conv, word_size));

		left_concat_labelFeat.forward(cg, &stance_label, &words_left_grnn._rnn_nodes[word_size - 1]);
		right_concat_labelFeat.forward(cg, &stance_label, &words_right_grnn._rnn_nodes[0]);

		word_left_lstm = &left_concat_labelFeat;
		word_right_lstm = &right_concat_labelFeat;
	}

};

#endif SRC_GlobalNodes_H_
