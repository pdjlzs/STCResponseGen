/*
 * CAction.h
 *
 *  Created on: Jan 25, 2016
 *      Author: mszhang
 */

#ifndef BASIC_CAction_H_
#define BASIC_CAction_H_



/*===============================================================
 *
 * scored actions
 *
 *==============================================================*/
// for segmentation, there are only threee valid operations
class CAction {
public:
	enum CODE { SEP = 0, APP = 1, FIN = 2, NO_ACTION = 3 };
	unsigned long _code;
	std::string _word;

public:
	CAction() : _code(NO_ACTION) {
		_word = "";
	}

	CAction(int code, const std::string& word) : _code(code){
		if (_code == SEP || _code == FIN) {
			_word = word;
		}
		else {
			_word = "";
		}
	}

	CAction(const CAction &ac) : _code(ac._code), _word(ac._word){
	}

public:
	inline void clear() { _code = NO_ACTION; _word = "";}

	inline void set(int code, const std::string& word) {
		_code = code;
		if (_code == SEP || _code == FIN) {
			_word = word;
		}
		else {
			_word = "";
		}
	}

	inline void set(const CAction &ac) {
		_code = ac._code;
		_word = ac._word;
	}

	inline bool isNone() const { return _code == NO_ACTION; }
	inline bool isSeparate() const { return _code == SEP; }
	inline bool isAppend() const { return _code == APP; }
	inline bool isFinish() const { return _code == FIN; }

public:
	inline std::string typestr() const {
		if (isNone()) { return "NONE"; }
		if (isSeparate()) { return "SEP"; }
		if (isAppend()) { return "APP"; }
		if (isFinish()) { return "FIN"; }
		return "NONE";
	}
	
	inline std::string str() const {
		if (isNone()) { return "NONE"; }
		if (isSeparate()) { return "SEP=" + _word; }
		if (isAppend()) { return "APP"; }
		if (isFinish()) { return "FIN=" + _word; }
		return "NONE";
	}	

public:
	const unsigned long &code() const { return _code; }
	const unsigned long &hash() const { return _code; }
	bool operator == (const CAction &a1) const { return _code == a1._code && _word.compare(a1._word) == 0; }
	bool operator != (const CAction &a1) const { return _code != a1._code || _word.compare(a1._word) != 0; }

};




inline std::ostream & operator << (std::ostream &os, const CAction &action) {
   os << action.str();
   return os;
}


#endif /* BASIC_CAction_H_ */
