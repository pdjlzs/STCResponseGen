/*
 * State.h
 *
 *  Created on: Oct 1, 2015
 *      Author: mszhang
 */

#ifndef SEG_STATE_H_
#define SEG_STATE_H_

#include "ModelParams.h"
#include "Action.h"
#include "ActionedNodes.h"
#include "AtomFeatures.h"
#include "Utf.h"
#include "GlobalNodes.h"
#include "IncrementalNodes.h"

#include <set>
class CStateItem {
public:
	std::string _word;
	CStateItem *_prevState;
	int _next_index;
	int _word_count;

	CAction _lastAction;
	PNode _score;

	// features
	ActionedNodes _nextscores;  // features current used
	AtomFeatures _atomFeat;  //features will be used for future
	IncrementalNodes _inc_nodes;

public:
	bool _bStart; // whether it is a start state
	bool _bGold; // for train

public:
	CStateItem() {
		clear();
	}


	virtual ~CStateItem(){
		clear();
	}

	void initial(ModelParams& params, HyperParams& hyparams, AlignedMemoryPool* mem) {
		_nextscores.resize(hyparams);
		_nextscores.initial(params, hyparams, mem);
	}


	void clear() {
		_word = "-start-";
		_prevState = 0;
		_next_index = 0;
		_word_count = 0;
		_lastAction.set(CAction::SEP, _word);
		_bStart = true;
		_bGold = true;
		_score = NULL;
	}


	const CStateItem* getPrevState() const{
		return _prevState;
	}

	std::string getLastWord() {
		return _word;
	}


public:
	//only assign context
	void separate(CStateItem* next, const string& rword){
		next->_word = rword;
		next->_prevState = this;
		next->_next_index = _next_index + 1;
		if (rword == "-end-")
			next->_word_count = _word_count;
		else
			next->_word_count = _word_count + 1;
		next->_lastAction.set(CAction::SEP, rword);
	}

	//only assign context
	void finish(CStateItem* next, const string& rword){
		next->_word = rword;
		next->_prevState = _prevState;
		next->_next_index = _next_index + 1;
		next->_word_count = _word_count;
		next->_lastAction.set(CAction::FIN, rword);
	}

	void idle(CStateItem* next, const string& rword){
		next->_word = "";
		next->_prevState = _prevState;
		next->_next_index = _next_index + 1;
		next->_word_count = _word_count;
		next->_lastAction.set(CAction::IDLE, rword);
	}


	void move(CStateItem* next, const CAction& ac){
		if (ac.isSeparate()) {
			separate(next, ac._word);
		}
		else if (ac.isFinish()) {
			finish(next, ac._word);
		}
		else if (ac.isIdle()) {
			idle(next, ac._word);
		}
		else {
			std::cout << "error action" << std::endl;
		}

		next->_bStart = false;
		next->_bGold = false;
	}

	bool IsTerminated() const {
		if (_lastAction.isFinish() || _lastAction.isIdle())
			return true;
		return false;
	}

	//partial results
	void getSegResults(std::vector<std::string>& resp_words) const {
		resp_words.clear();
		if (!IsTerminated()) {
			resp_words.insert(resp_words.begin(), _word);
		}

		const CStateItem *prevState = this->_prevState;
		while (prevState != 0 && prevState->_word != "-end-" && prevState->_word != "" && prevState->_word != "-start-") {
			resp_words.insert(resp_words.begin(), prevState->_word);
			prevState = prevState->_prevState;
		}

		if (resp_words.size() != _word_count) {
			std::cout << "bug exists: " << resp_words.size() << " " << _word_count << std::endl;
		}
	}

	void getGoldAction(const std::vector<std::string>& resp_words, CAction& ac) const {
		if (_next_index > resp_words.size()){
			ac.set(CAction::FIN, "");
			return;
		}

		if (_next_index == resp_words.size()){
			ac.set(CAction::SEP, "-end-");
			return;
		}

		if (_next_index >= 0 && _next_index < resp_words.size()) {
			// should have a check here to see whether the words are match, but I did not do it here
			ac.set(CAction::SEP, resp_words[_word_count]);
			return;
		}

		ac.set(CAction::NO_ACTION, "");
		return;
	}


	void getCandidateActions(vector<CAction> & actions, int post_word_size, HyperParams* opts) const{
		actions.clear();
		static CAction ac;
		unordered_map<string, vector<string> >& word_map = opts->trigram_candid;
		unordered_map<string, vector<int> > &rand_tabel = opts->random_tabel;
		if (_word == "-end-"){
			ac.set(CAction::FIN, "");
			actions.push_back(ac);
		}
		else if (_word == ""){
			ac.set(CAction::IDLE, "");
			actions.push_back(ac);
		}
		else if (_next_index >= opts->maxlength){
			ac.set(CAction::SEP, "-end-");
			actions.push_back(ac);
		}
		else {
			string preword = _prevState == 0 ? "-start-" : this->_prevState->_word;
			string wordbigram = preword + seperateKey + _word;
			if (word_map.find(wordbigram) != word_map.end()) {
				vector<string>& wordCandi = word_map[wordbigram];
				int candi_size = wordCandi.size();
				int max_candid = opts->maxCandidAction;
				std::set<int> index_set;

				if (candi_size > max_candid){
					vector<int>& wordCandiTabel = rand_tabel[wordbigram];
					random_shuffle(wordCandiTabel.begin(), wordCandiTabel.end());
					for (int i = 0; index_set.size() < max_candid; i++){
						index_set.insert(wordCandiTabel[i]);
					}
				}
				else {
					for (int i = 0; i < wordCandi.size(); i++){
						index_set.insert(i);
					}
				}
				set<int>::iterator it;
				/*
				//debug
				for (it = index_set.begin(); it != index_set.end(); it++)
				cout << *it << " ";
				cout << endl;*/

				for (it = index_set.begin(); it != index_set.end(); it++){
					ac.set(CAction::SEP, wordCandi[*it]);
					actions.push_back(ac);
				}

			}
			//else std::cout << wordbigram << " is not in dic" << endl;
			if (actions.size() == 0){
				ac.set(CAction::SEP, "-end-");
				actions.push_back(ac);
			}
		}

	}

	inline std::string str() const{
		stringstream curoutstr;
		curoutstr << "score: " << _score->val[0] << " ";
		curoutstr << "respond words:";
		std::vector<std::string> resp_words;
		getSegResults(resp_words);
		for (int idx = 0; idx < resp_words.size(); idx++){
			curoutstr << " " << resp_words[idx];
		}

		curoutstr << "actions:";
		vector<string> allacs;

		const CStateItem * curState;
		curState = this;
		while (!curState->_bStart) {
			allacs.insert(allacs.begin(), curState->_lastAction.str());
			curState = curState->_prevState;
		}
		for (int idx = 0; idx < allacs.size(); idx++) {
			curoutstr << " " << allacs[idx];
		}
		return curoutstr.str();
	}


public:
	inline void computeNextScore(Graph *cg, const vector<CAction>& acs){
		/*
		if (_bStart){
		_nextscores.forward(cg, acs, _atomFeat, NULL);
		}
		else{
		_nextscores.forward(cg, acs, _atomFeat, _score);
		}*/
		_nextscores.forward(cg, acs, _atomFeat);

	}

	inline void prepare(HyperParams* hyper_params, ModelParams* model_params, GlobalNodes& global_nodes){

		_atomFeat.str_1AC = _lastAction.str();
		_atomFeat.str_2AC = _prevState == 0 ? nullkey : _prevState->_lastAction.str();
		_atomFeat.pre_actions_lstm = _prevState == 0 ? NULL : &(_prevState->_nextscores.action_lstm);
		_atomFeat.post_words_left_lstm = global_nodes.word_left_lstm;
		_atomFeat.post_words_right_lstm = global_nodes.word_right_lstm;
		_atomFeat.next_position = _next_index;
	}
};


class CScoredState {
public:
	CStateItem *item;
	CAction ac;
	dtype score;
	bool bGold;
	int position;

public:
	CScoredState() : item(0), score(0), ac(0, ""), bGold(0), position(-1){
	}

	CScoredState(const CScoredState& other) : item(other.item), score(other.score), ac(other.ac), bGold(other.bGold), position(other.position) {

	}

public:
	bool operator <(const CScoredState &a1) const {
		return score < a1.score;
	}
	bool operator >(const CScoredState &a1) const {
		return score > a1.score;
	}
	bool operator <=(const CScoredState &a1) const {
		return score <= a1.score;
	}
	bool operator >=(const CScoredState &a1) const {
		return score >= a1.score;
	}
};

class CScoredState_Compare {
public:
	int operator()(const CScoredState &o1, const CScoredState &o2) const {

		if (o1.score < o2.score)
			return -1;
		else if (o1.score > o2.score)
			return 1;
		else
			return 0;
	}
};


#endif /* SEG_STATE_H_ */
