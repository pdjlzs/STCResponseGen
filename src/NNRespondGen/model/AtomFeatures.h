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
	int next_position;
public:
	IncLSTM1Builder* pre_words_lstm;
	IncLSTM1Builder* pre_actions_lstm;
public:
	void clear(){
		str_1AC = "";
		str_2AC = "";
		next_position = -1;
		pre_words_lstm = NULL;
		pre_actions_lstm = NULL;
	}

};

#endif /* SRC_AtomFeatures_H_ */
