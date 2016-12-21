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

class CStateItem {
public:
	std::string _word;
	int _wstart;
	int _wend;
	CStateItem *_prevStartWordState;
	CStateItem *_prevState;
	int _next_index;

	const std::vector<std::string> *_chars;
	int _char_size;
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
		_nextscores.initial(params, hyparams, mem);
	}

	void setInput(const std::vector<std::string>* pCharacters) {
		_chars = pCharacters;
		_char_size = pCharacters->size();
	}

	void clear() {
		_word = "";
		_wstart = -1;
		_wend = -1;
		_prevStartWordState = 0;
		_prevState = 0;
		_next_index = 0;
		_chars = 0;
		_char_size = 0;
		_lastAction.clear();
		_word_count = 0;
		_bStart = true;
		_bGold = true;
		_score = NULL;
	}


	const CStateItem* getPrevStackState() const{
		return _prevStartWordState;
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
		if (_next_index >= _char_size) {
			std::cout << "separate error" << std::endl;
			return;
		}
		next->_word = (*_chars)[_next_index];
		next->_wstart = _next_index;
		next->_wend = _next_index;
		next->_prevStartWordState = next;
		next->_prevState = this;
		next->_next_index = _next_index + 1;
		next->_chars = _chars;
		next->_char_size = _char_size;
		next->_word_count = _word_count + 1;
		next->_lastAction.set(CAction::SEP, rword);
	}

	//only assign context
	void finish(CStateItem* next, const string& rword){
		if (_next_index != _char_size) {
			std::cout << "finish error" << std::endl;
			return;
		}
		next->_word = "";
		next->_wstart = -1;
		next->_wend = -1;
		next->_prevStartWordState = next;
		next->_prevState = this;
		next->_next_index = _next_index + 1;
		next->_chars = _chars;
		next->_char_size = _char_size;
		next->_word_count = _word_count;
		next->_lastAction.set(CAction::FIN, rword);
	}

	//only assign context
	void append(CStateItem* next){
		if (_next_index >= _char_size) {
			std::cout << "append error" << std::endl;
			return;
		}
		next->_word = _word + (*_chars)[_next_index];
		next->_wstart = _wstart;
		next->_wend = _next_index;
		next->_prevStartWordState = _prevStartWordState;
		next->_prevState = this;
		next->_next_index = _next_index + 1;
		next->_chars = _chars;
		next->_char_size = _char_size;
		next->_word_count = _word_count;
		next->_lastAction.set(CAction::APP, "");
	}

	void move(CStateItem* next, const CAction& ac){
		if (ac.isAppend()) {
			append(next);
		}
		else if (ac.isSeparate()) {
			separate(next, ac._word);
		}
		else if (ac.isFinish()) {
			finish(next, ac._word);
		}
		else {
			std::cout << "error action" << std::endl;
		}

		next->_bStart = false;
		next->_bGold = false;
	}

	bool IsTerminated() const {
		if (_lastAction.isFinish())
			return true;
		return false;
	}

	//partial results
	void getSegResults(std::vector<std::string>& words, std::vector<std::string>& normwords) const {
		words.clear();  normwords.clear();
		if (!IsTerminated()) {
			normwords.insert(normwords.begin(), _word);
			words.insert(words.begin(), _word);
		}

		const CStateItem *prevStartWordState = _prevStartWordState;
		while (prevStartWordState != 0 && prevStartWordState->_lastAction._word != "") {
			normwords.insert(normwords.begin(), prevStartWordState->_lastAction._word);
			words.insert(words.begin(), prevStartWordState->_prevState->_word);
			prevStartWordState = prevStartWordState->_prevState->_prevStartWordState;
		}

		if (normwords.size() != _word_count || words.size() != _word_count) {
			std::cout << "bug exists: " << normwords.size() << " " << _word_count << std::endl;
		}
	}

	void getGoldAction(const std::vector<std::string>& segments, const std::vector<std::string>& normsegments, CAction& ac) const {
		if (_next_index == _char_size) {
			ac.set(CAction::FIN, normsegments[_word_count - 1]);
			return;
		}
		if (_next_index == 0) {
			ac.set(CAction::SEP, "");
			return;
		}

		if (_next_index > 0 && _next_index < _char_size) {
			// should have a check here to see whether the words are match, but I did not do it here
			if (_word.length() == segments[_word_count - 1].length()) {
				ac.set(CAction::SEP, normsegments[_word_count - 1]);
				return;
			}
			else {
				ac.set(CAction::APP, "");
				return;
			}
		}

		ac.set(CAction::NO_ACTION, "");
		return;
	}

	// we did not judge whether history actions are match with current state.
	void getGoldAction(const CStateItem* goldState, CAction& ac) const{
		if (_next_index > goldState->_next_index || _next_index < 0) {
			ac.set(CAction::NO_ACTION, "");
			return;
		}
		const CStateItem *prevState = goldState->_prevState;
		CAction curAction = goldState->_lastAction;
		while (_next_index < prevState->_next_index) {
			curAction = prevState->_lastAction;
			prevState = prevState->_prevState;
		}
		return ac.set(curAction._code, curAction._word);
	}

	void getCandidateActions(vector<CAction> & actions, HyperParams* pHyparams) const{
		actions.clear();
		static CAction ac;
		if (_next_index == 0){
			ac.set(CAction::SEP, "");
			actions.push_back(ac);
		}
		else if (_next_index == _char_size){
			ac.set(CAction::FIN, _word);
			actions.push_back(ac);
			if (pHyparams->word_map.find(_word) != pHyparams->word_map.end()) {
				for (int idy = 0; idy < pHyparams->word_map[_word].size(); idy++) {
					ac.set(CAction::FIN, pHyparams->word_map[_word][idy]);
					actions.push_back(ac);
				}
			}
		}
		else if (_next_index > 0 && _next_index < _char_size){
			ac.set(CAction::SEP, _word);
			actions.push_back(ac);
			if (pHyparams->word_map.find(_word) != pHyparams->word_map.end()) {
				for (int idy = 0; idy < pHyparams->word_map[_word].size(); idy++) {
					ac.set(CAction::SEP, pHyparams->word_map[_word][idy]);
					actions.push_back(ac);
				}
			}
			ac.set(CAction::APP, "");
			actions.push_back(ac);
		}
		else{

		}

	}

	inline std::string str() const{
		stringstream curoutstr;
		curoutstr << "score: " << _score->val[0] << " ";
		curoutstr << "seg:";
		std::vector<std::string> words, normwords;
		getSegResults(words, normwords);
		for (int idx = 0; idx < words.size(); idx++){
			curoutstr << " " << words[idx] << "_"  << normwords[idx];
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
		if (_bStart){
			_nextscores.forward(cg, acs, _atomFeat, NULL);
		}
		else{
			_nextscores.forward(cg, acs, _atomFeat, _score);
		}
	}

	inline void prepare(HyperParams* hyper_params, ModelParams* model_params, GlobalNodes* global_nodes){
		_atomFeat.str_2W = nullkey;
		if(_prevStartWordState != 0 && !_prevStartWordState->_bStart){
			_atomFeat.str_2W = _prevStartWordState->_lastAction._word;			
		}

		_atomFeat.str_1AC = _lastAction.typestr();
		_atomFeat.str_2AC = _prevState == 0 ?  nullkey: _prevState->_lastAction.typestr();
		_atomFeat.p_action_lstm = _prevState == 0 ? NULL : &(_prevState->_nextscores.action_lstm);
		_atomFeat.p_word_lstm = _inc_nodes.word_lstm;
		_atomFeat.next_position = _next_index >= 0 && _next_index < _char_size ? _next_index : -1;
		_atomFeat.p_char_left_lstm = global_nodes == NULL ? NULL : &(global_nodes->char_left_lstm);
		_atomFeat.p_char_right_lstm = global_nodes == NULL ? NULL : &(global_nodes->char_right_lstm);
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
