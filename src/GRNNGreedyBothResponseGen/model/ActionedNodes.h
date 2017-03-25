#ifndef SRC_ActionedNodes_H_
#define SRC_ActionedNodes_H_

#include "ModelParams.h"
#include "AtomFeatures.h"

struct ActionedNodes {
	LookupNode last_action_input;
	LookupNode last2_action_input;
	BiNode action_conv;
	IncLSTM1Builder action_lstm;

	ConcatNode state_represent;
	UniNode state_hidden;

	vector<LookupNode> current_action_input;

	vector<PDotNode> action_score;
	vector<SPAddNode> outputs;

public:
	inline void resize(HyperParams& opts){
		current_action_input.resize(opts.action_num);
		action_score.resize(opts.action_num);
		outputs.resize(opts.action_num);
	}
	inline void initial(ModelParams &params, HyperParams &opts, AlignedMemoryPool *mem) {

		last_action_input.setParam(&(params.action_table));
		last_action_input.init(opts.action_dim, opts.dropProb, mem);
		last2_action_input.setParam(&(params.action_table));
		last2_action_input.init(opts.action_dim, opts.dropProb, mem);
		action_conv.setParam(&(params.action_conv));
		action_conv.init(opts.action_hiddensize, opts.dropProb, mem);
		action_lstm.init(&(params.action_lstm), opts.dropProb, mem);

		state_represent.init(opts.state_represent_dim, -1, mem);
		state_hidden.setParam(&(params.state_hidden));
		state_hidden.init(opts.state_hiddensize, -1, mem);

		for (int idx = 0; idx < opts.action_num; idx++) {
			current_action_input[idx].setParam(&(params.scored_action_table));
			current_action_input[idx].init(opts.state_hiddensize, opts.dropProb, mem);

			action_score[idx].init(1, -1, mem);
			outputs[idx].init(1, -1, mem);
		}

	}


public:
	inline void forward(Graph *cg, const vector<CAction> &actions, const AtomFeatures &atomFeat) {
		static vector<PNode> sumNodes;
		static CAction ac;
		static int ac_num;
		static vector<PNode> states;
		ac_num = actions.size();

		last_action_input.forward(cg, atomFeat.str_1AC);
		last2_action_input.forward(cg, atomFeat.str_2AC);

		action_conv.forward(cg, &last2_action_input, &last_action_input);
		action_lstm.forward(cg, &action_conv, atomFeat.pre_actions_lstm);

		states.clear();
		states.push_back(atomFeat.post_words_left_lstm);
		states.push_back(atomFeat.post_words_right_lstm);
		states.push_back(&action_lstm._hidden);

		state_represent.forward(cg, states);
		state_hidden.forward(cg, &state_represent);

		for (int idx = 0; idx < ac_num; idx++) {
			ac.set(actions[idx]);

			string action_name = ac.str();
			sumNodes.clear();

			current_action_input[idx].forward(cg, action_name);
			action_score[idx].forward(cg, &current_action_input[idx], &state_hidden);
			sumNodes.push_back(&action_score[idx]);

			outputs[idx].forward(cg, sumNodes, 0);
		}
	}
};


#endif SRC_ActionedNodes_H_
