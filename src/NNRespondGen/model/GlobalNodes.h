/*
 * Feature.h
 *
 *  Created on: Aug 25, 2016
 *      Author: mszhang
 */

#ifndef SRC_GlobalNodes_H_
#define SRC_GlobalNodes_H_

#include "ModelParams.h"

struct GlobalNodes {
	vector<LookupNode> char_inputs;
	vector<LookupNode> chartype_inputs;
	vector<LookupNode> bichar_inputs;
	vector<ConcatNode> char_represents;
	WindowBuilder char_window;
	vector<UniNode> char_tanh_conv;
	LSTM1Builder char_left_lstm;
	LSTM1Builder char_right_lstm;


	Node _bucket;

public:
	inline void resize(int max_length){
		char_inputs.resize(max_length);
		bichar_inputs.resize(max_length);
		chartype_inputs.resize(max_length);
		char_represents.resize(max_length);
		char_window.resize(max_length);
		char_tanh_conv.resize(max_length);
		char_left_lstm.resize(max_length);
		char_right_lstm.resize(max_length);
	}

public:
	inline void initial(ModelParams& params, HyperParams& hyparams, AlignedMemoryPool* mem){
		int length = char_inputs.size();
		for (int idx = 0; idx < length; idx++){
			char_inputs[idx].setParam(&params.char_table);
			bichar_inputs[idx].setParam(&params.bichar_table);
			chartype_inputs[idx].setParam(&params.chartype_table);
			char_tanh_conv[idx].setParam(&params.char_tanh_conv);
		}

		char_window.init(hyparams.char_repsentation_dim, hyparams.char_context, mem);
		char_left_lstm.init(&params.char_left_lstm, hyparams.dropProb, true, mem);
		char_right_lstm.init(&params.char_right_lstm, hyparams.dropProb, true, mem);

		for (int idx = 0; idx < length; idx++) {
			char_inputs[idx].init(hyparams.char_dim, hyparams.dropProb, mem);
			bichar_inputs[idx].init(hyparams.bichar_dim, hyparams.dropProb, mem);
			chartype_inputs[idx].init(hyparams.chartype_dim, hyparams.dropProb, mem);
			char_represents[idx].init(hyparams.char_repsentation_dim, -1, mem);
			char_tanh_conv[idx].init(hyparams.char_hidden_dim, hyparams.dropProb, mem);
		}

		_bucket.init(hyparams.char_repsentation_dim, -1, mem);
	}


public:
	inline void forward(Graph* cg, const std::vector<std::string>* pCharacters){
		int char_size = pCharacters->size();
		string unichar, biChar, chartype;
		for (int idx = 0; idx < char_size; idx++){
			unichar = (*pCharacters)[idx];
			biChar = idx < char_size - 1 ? (*pCharacters)[idx] + (*pCharacters)[idx + 1] : nullkey;
			chartype = wordtype(unichar);
			char_inputs[idx].forward(cg, unichar);
			bichar_inputs[idx].forward(cg, biChar);
			chartype_inputs[idx].forward(cg, chartype);
			char_represents[idx].forward(cg, &(char_inputs[idx]), &(bichar_inputs[idx]), &(chartype_inputs[idx]));
		}
		char_window.forward(cg, getPNodes(char_represents, char_size));
		for (int idx = 0; idx < char_size; idx++) {
			char_tanh_conv[idx].forward(cg, &(char_window._outputs[idx]));
		}
		char_left_lstm.forward(cg, getPNodes(char_tanh_conv, char_size));
		char_right_lstm.forward(cg, getPNodes(char_tanh_conv, char_size));
	}

};

#endif /* SRC_GlobalNodes_H_ */
