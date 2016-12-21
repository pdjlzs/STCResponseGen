/*
 * Normalizer.h
 *
 *  Created on: Jan 25, 2016
 *      Author: mszhang
 */

#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_

#include "N3L.h"

#include "Driver.h"
#include "Options.h"
#include "Pipe.h"
#include "Utf.h"

using namespace nr;
using namespace std;


class Normalizer {
public:
	Normalizer(size_t memsize);
	virtual ~Normalizer();

public:
	Driver m_driver;
	Options m_options;
	Pipe m_pipe;

public:
	int createAlphabet(const vector<Instance>& vecInsts);
	int initialActionWordMap();


public:
	void train(const string& trainFile, const string& devFile, const string& testFile, const string& modelFile, const string& optionFile);
	void predict(const Instance& input, vector<string>& output, vector<string>& normoutput);
	void test(const string& testFile, const string& outputFile, const string& modelFile);

	// static training
	void getGoldActions(const vector<Instance>& vecInsts, vector<vector<CAction> >& vecActions);
	void getGoldActions(const Instance& inst, vector<CAction>& actions);

public:
	void writeModelFile(const string& outputModelFile);
	void loadModelFile(const string& inputModelFile);

protected:
	inline void randomInput(const Instance& inst, Instance& outInst) {
		outInst.clear();
		dtype rpRation = m_options.rpRatio;
		vector<string> curChars;
		for (int idx = 0; idx < inst.words.size(); idx++) {
			dtype curRand = dtype(rand()) / RAND_MAX;
			string curWord = inst.words[idx];
			string newWord = inst.words[idx];
			if (curRand < rpRation 
			&& m_driver._hyperparams.reverse_word_map.find(curWord) != m_driver._hyperparams.reverse_word_map.end()) {
				int selId = rand() % m_driver._hyperparams.reverse_word_map[curWord].size();
				newWord = m_driver._hyperparams.reverse_word_map[curWord][selId];
			}

			getCharactersFromUTF8String(newWord, curChars);	
			for (int idy = 0; idy < curChars.size(); idy++) {
				outInst.chars.push_back(curChars[idy]);
			}
			outInst.words.push_back(newWord);
			outInst.normwords.push_back(curWord);
		}
	}

};

#endif /* SRC_PARSER_H_ */
