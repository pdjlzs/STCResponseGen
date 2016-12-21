/*
 * Feature.h
 *
 *  Created on: Aug 25, 2016
 *      Author: mszhang
 */

#ifndef SRC_ActionedNodes_H_
#define SRC_ActionedNodes_H_

#include "ModelParams.h"
#include "AtomFeatures.h"

struct ActionedNodes {
	vector<LookupNode> last_word_input;
	LookupNode last2_word_input;
	vector<BiNode> word_conv;
	vector<IncLSTM1Builder> word_lstm;

	LookupNode last_action_input;
	LookupNode last2_action_input;
	BiNode action_conv;
	IncLSTM1Builder action_lstm;
	
	vector<FourNode> sep_hidden;
	TriNode app_hidden;
	vector<LinearNode> sep_score;
	LinearNode app_score;

	vector<SPAddNode> outputs;

	Node bucket;


public:
	inline void initial(ModelParams& params, HyperParams& hyparams, AlignedMemoryPool* mem){
		last_word_input.resize(hyparams.action_num);
		word_conv.resize(hyparams.action_num);
		word_lstm.resize(hyparams.action_num);
		sep_hidden.resize(hyparams.action_num);
		sep_score.resize(hyparams.action_num);
		outputs.resize(hyparams.action_num);

		//neural features
		for (int idx = 0; idx < hyparams.action_num; idx++) {
			last_word_input[idx].setParam(&(params.word_table));
			last_word_input[idx].init(hyparams.word_dim, hyparams.dropProb, mem);
			word_conv[idx].setParam(&(params.word_conv));
			word_conv[idx].init(hyparams.word_hidden_dim, hyparams.dropProb, mem);
			word_lstm[idx].init(&(params.word_lstm), hyparams.dropProb, mem); //already allocated here

			sep_hidden[idx].setParam(&(params.sep_hidden));
			sep_hidden[idx].init(hyparams.sep_hidden_dim, hyparams.dropProb, mem);
			sep_score[idx].setParam(&(params.sep_score));
			sep_score[idx].init(1, -1, mem);

			outputs[idx].init(1, -1, mem);
		}
		
		last2_word_input.setParam(&(params.word_table));
		last2_word_input.init(hyparams.word_dim, hyparams.dropProb, mem);

		last_action_input.setParam(&(params.action_table));
		last_action_input.init(hyparams.action_dim, hyparams.dropProb, mem);
		last2_action_input.setParam(&(params.action_table));
		last2_action_input.init(hyparams.action_dim, hyparams.dropProb, mem);
		action_conv.setParam(&(params.action_conv));
		action_conv.init(hyparams.action_hidden_dim, hyparams.dropProb, mem);
		action_lstm.init(&(params.action_lstm), hyparams.dropProb, mem); //already allocated here
		
		app_hidden.setParam(&(params.app_hidden));
		app_hidden.init(hyparams.app_hidden_dim, hyparams.dropProb, mem);
		app_score.setParam(&(params.app_score));
		app_score.init(1, -1, mem);

		bucket.init(hyparams.char_lstm_dim, -1, mem);
		
	}


public:
	inline void forward(Graph* cg, const vector<CAction>& actions, AtomFeatures& atomFeat, PNode prevStateNode){
		static vector<PNode> sumNodes;
		static CAction ac;
		static int ac_num;
		ac_num = actions.size();

		last2_action_input.forward(cg, atomFeat.str_2AC);
		last_action_input.forward(cg, atomFeat.str_1AC);
		action_conv.forward(cg, &last2_action_input, &last_action_input);
		action_lstm.forward(cg, &action_conv, atomFeat.p_action_lstm);

		last2_word_input.forward(cg, atomFeat.str_2W);

		PNode P_char_left_lstm = atomFeat.next_position >= 0 ? &(atomFeat.p_char_left_lstm->_hiddens[atomFeat.next_position]) : &bucket;
		PNode P_char_right_lstm = atomFeat.next_position >= 0 ? &(atomFeat.p_char_right_lstm->_hiddens[atomFeat.next_position]) : &bucket;


		for (int idx = 0; idx < ac_num; idx++){
			ac.set(actions[idx]);
			sumNodes.clear();

			if (prevStateNode != NULL){
				sumNodes.push_back(prevStateNode);
			}

			//neural features
			if (ac.isAppend()){
				app_hidden.forward(cg, P_char_left_lstm, P_char_right_lstm, &(action_lstm._hidden));
				app_score.forward(cg, &app_hidden);
				sumNodes.push_back(&app_score);
			}
			else{
				last_word_input[idx].forward(cg, ac._word == "" ? nullkey : ac._word);
				word_conv[idx].forward(cg, &last2_word_input, &last_word_input[idx]);
				word_lstm[idx].forward(cg, &word_conv[idx], atomFeat.p_word_lstm);

				sep_hidden[idx].forward(cg, P_char_left_lstm, P_char_right_lstm, &(word_lstm[idx]._hidden), &(action_lstm._hidden));
				sep_score[idx].forward(cg, &sep_hidden[idx]);
				sumNodes.push_back(&sep_score[idx]);
			}

			outputs[idx].forward(cg, sumNodes, ac._code);
		}
	}

};

#endif /* SRC_ActionedNodes_H_ */
