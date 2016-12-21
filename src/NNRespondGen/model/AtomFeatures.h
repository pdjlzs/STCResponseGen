/*
 * AtomFeatures.h
 *
 *  Created on: Aug 25, 2016
 *      Author: mszhang
 */

#ifndef SRC_AtomFeatures_H_
#define SRC_AtomFeatures_H_
#include "ModelParams.h"
struct AtomFeatures {
	string str_1AC;
	string str_2AC;
	string str_2W;
	int next_position;
public:
	IncLSTM1Builder* p_word_lstm;
	IncLSTM1Builder* p_action_lstm;
	LSTM1Builder* p_char_left_lstm;
	LSTM1Builder* p_char_right_lstm;
public:
	void clear(){
		str_2W = "";
		str_1AC = "";
		str_2AC = "";
		next_position = -1;
		p_word_lstm = NULL;
		p_action_lstm = NULL;
		p_char_left_lstm = NULL;
		p_char_right_lstm = NULL;
	}

};

#endif /* SRC_AtomFeatures_H_ */
