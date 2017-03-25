/*
 * ActionFeatures.h
 *
 *  Created on: Aug 25, 2016
 *      Author: mszhang
 */

#ifndef SRC_ActionFeatures_H_
#define SRC_ActionFeatures_H_
#include "ModelParams.h"
#include "Action.h"
#include "AtomFeatures.h"
#include "Utf.h"
struct ActionFeatures {
public:
	string str_1C;  //equals str_1Wcn
	string str_2C; 

	string str_1CT;  //equals str_1Wcn
	string str_2CT;

	string str_1W;
	string str_1Wc0;
	vector<string> str_1Wci;

public:
	int sid_1WL;

public:
	int sid_1C;  //equals str_1Wcn
	int sid_2C;

	int sid_1CT;  //equals str_1Wcn
	int sid_2CT;

	int sid_1W;
	int sid_1Wc0;
	vector<int> sid_1Wci;

public:
	void clear(){
		str_1C = "";
		str_2C = "";

		str_1CT = "";
		str_2CT = "";

		str_1W = "";
		str_1Wc0 = "";
		str_1Wci.clear();


		sid_1C = -1;
		sid_2C = -1;

		//ids
		sid_1CT = -1;
		sid_2CT = -1;

		sid_1W = -1;
		sid_1WL = -1;
		sid_1Wc0 = -1;
		sid_1Wci.clear();
	}

public:
	void init(AtomFeatures* pAtomFeat, const CAction& ac, HyperParams* opts, ModelParams* model){
		if (ac._word.length() == 0){
			str_1C = pAtomFeat->str_1C;
			str_2C = pAtomFeat->str_2C;

			str_1CT = pAtomFeat->str_1CT;
			str_2CT = pAtomFeat->str_2CT;

			str_1W = pAtomFeat->str_1W;
			str_1Wc0 = pAtomFeat->str_1Wc0;
			str_1Wci.clear();
			for (int idx = 0; idx < pAtomFeat->str_1Wci.size(); idx++) {
				str_1Wci.push_back(pAtomFeat->str_1Wci[idx]);
			}


			sid_1C = pAtomFeat->sid_1C;
			sid_2C = pAtomFeat->sid_2C;

			//ids
			sid_1CT = pAtomFeat->sid_1CT;
			sid_2CT = pAtomFeat->sid_2CT;

			sid_1W = pAtomFeat->sid_1W;
			sid_1WL = pAtomFeat->sid_1WL;
			sid_1Wc0 = pAtomFeat->sid_1Wc0;
			sid_1Wci.clear();
			for (int idx = 0; idx < pAtomFeat->sid_1Wci.size(); idx++) {
				sid_1Wci.push_back(pAtomFeat->sid_1Wci[idx]);
			}
		}
		else {
			vector<string> chars;
			getCharactersFromUTF8String(ac._word, chars);
 			str_1C = chars[chars.size()-1];
			str_2C = chars.size() > 1 ? chars[chars.size() - 2] : pAtomFeat->str_2Wcn;

			str_1CT = wordtype(str_1C);
			str_2CT = wordtype(str_2C);

			str_1W = ac._word;
			str_1Wc0 = chars[0];
			str_1Wci.clear();
			for (int idx = 0; idx < pAtomFeat->str_1Wci.size(); idx++) {
				str_1Wci.push_back(chars[idx]);
			}


			sid_1C = model->chars.from_string(str_1C);
			sid_2C = model->chars.from_string(str_2C);

			sid_1CT = model->charTypes.from_string(str_1CT);
			sid_2CT = model->charTypes.from_string(str_2CT);

			sid_1W = model->words.from_string(str_1W);
			sid_1Wc0 = model->chars.from_string(str_1Wc0);
			sid_1Wci.resize(str_1Wci.size());
			for (int idx = 0; idx < str_1Wci.size(); idx++) {
				sid_1Wci[idx] = model->chars.from_string(str_1Wci[idx]);
			}
			sid_1WL = chars.size() > 5 ?  5 : chars.size();
		}

	}

};

#endif /* SRC_ActionFeatures_H_ */
