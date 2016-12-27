#ifndef SRC_ActionedNodes_H_
#define SRC_ActionedNodes_H_

#include "ModelParams.h"
#include "AtomFeatures.h"

struct ActionedNodes {
	LookupNode last_action_input;
	LookupNode last2_action_input;
	BiNode action_conv;
	IncLSTM1Builder action_lstm;

	LookupNode last_word_input;
	LookupNode last2_word_input;
	BiNode word_conv;
	IncLSTM1Builder word_lstm;

	ConcatNode state_represent;
	UniNode state_hidden;

	vector<LookupNode> current_action_input;

	vector<PDotNode> action_score;
	vector<SPAddNode> outputs;

public:
	inline void initial(ModelParams &params, HyperParams &hyparams, AlignedMemoryPool *mem) {
		action_score.resize(hyparams.action_num);
		outputs.resize(hyparams.action_num);
		current_action_input.resize(hyparams.action_num);

		//neural features
		for (int idx = 0; idx < hyparams.action_num; idx++) {
			current_action_input[idx].setParam(&(params.scored_action_table));
			current_action_input[idx].init(hyparams.state_hidden_dim, hyparams.dropProb, mem);

			action_score[idx].init(1, -1, mem);
			outputs[idx].init(1, -1, mem);
		}

		last_action_input.setParam(&(params.action_table));
		last_action_input.init(hyparams.action_dim, hyparams.dropProb, mem);
		last2_action_input.setParam(&(params.action_table));
		last2_action_input.init(hyparams.action_dim, hyparams.dropProb, mem);
		action_conv.setParam(&(params.action_conv));
		action_conv.init(hyparams.action_hidden_dim, hyparams.dropProb, mem);
		action_lstm.init(&(params.action_lstm), hyparams.dropProb, mem); 

		last_word_input.setParam(&(params.word_table));
		last2_word_input.setParam(&(params.word_table));
		word_conv.setParam(&(params.word_conv));
		word_lstm.init(&(params.word_lstm), hyparams.dropProb, mem);

		state_represent.init(hyparams.state_concat_dim, -1, mem);
		state_hidden.setParam(&(params.state_hidden));
		state_hidden.init(hyparams.state_hidden_dim, -1, mem);

	}


public:
	inline void forward(Graph *cg, const vector<CAction> &actions, const AtomFeatures &atomFeat, PNode prevStateNode) {
		static vector<PNode> sumNodes;
		static CAction ac;
		static int ac_num;
		static vector<PNode> states;
		ac_num = actions.size();
		last2_action_input.forward(cg, atomFeat.str_2AC);
		last_action_input.forward(cg, atomFeat.str_1AC);

		last_action_concat.forward(cg, &last2_action_input, &last_action_input);
		action_lstm.forward(cg, &last_action_concat, atomFeat.pre_actions_lstm);

		word_lstm.forward(cg, )

		states.clear();
		states.push_back(&(atomFeat.pre_words_lstm->_hidden));
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
