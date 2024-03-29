#ifndef _PARSER_OPTIONS_
#define _PARSER_OPTIONS_

#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "N3L.h"

using namespace std;

class Options {
public:
	int wordCutOff;
	int featCutOff;
	int maxIter;
	int batchSize;
	dtype adaEps;
	dtype adaAlpha;
	dtype regParameter;
	dtype dropProb;
	dtype delta;
	int beam;

	string wordEmbFile;
	int wordEmbSize;
	bool wordEmbFineTune;
	int wordHiddenSize;
	int wordRNNHiddenSize;

	int wordcontext;
	string mapFile;
	string lmFile;

	int actionEmbSize;
	int actionHiddenSize;
	int actionRNNHiddenSize;

	int state_hiddensize;

	int verboseIter;
	bool train;
	int maxInstance;
	vector<string> testFiles;
	string outBest;

	int maxLength;
	int maxCandidAction;
	
	int unkStrategy;

	int labelFeatEmbSize;
	bool labelFeatEmbFineTune;

	Options() {
		wordCutOff = 0;
		featCutOff = 0;
		maxIter = 1000;
		batchSize = 1;
		adaEps = 1e-6;
		adaAlpha = 0.01;
		regParameter = 1e-8;
		dropProb = -1;
		delta = 0.2;
		beam = 16;

		state_hiddensize = 100;

		wordEmbSize = 100;
		wordHiddenSize = 300;
		wordRNNHiddenSize = 300;
		wordEmbFineTune = false;
		wordEmbFile = "";

		wordcontext = 2;
		mapFile = "";
		lmFile = "";

		actionEmbSize = 50;
		actionHiddenSize = 100;
		actionRNNHiddenSize = 100;

		verboseIter = 100;
		train = false;
		maxInstance = -1;
		testFiles.clear();
		outBest = "";
		
		maxLength = 28;
		maxCandidAction = 100;

		unkStrategy = 0;

		labelFeatEmbSize = 50;
		labelFeatEmbFineTune = true;

	}

	virtual ~Options() {

	}

	void setOptions(const vector<string> &vecOption) {
		int i = 0;
		for (; i < vecOption.size(); ++i) {
			pair<string, string> pr;
			string2pair(vecOption[i], pr, '=');
			if (pr.first == "wordCutOff")
				wordCutOff = atoi(pr.second.c_str());
			if (pr.first == "featCutOff")
				featCutOff = atoi(pr.second.c_str());
			if (pr.first == "maxIter")
				maxIter = atoi(pr.second.c_str());
			if (pr.first == "batchSize")
				batchSize = atoi(pr.second.c_str());
			if (pr.first == "adaEps")
				adaEps = atof(pr.second.c_str());
			if (pr.first == "adaAlpha")
				adaAlpha = atof(pr.second.c_str());
			if (pr.first == "regParameter")
				regParameter = atof(pr.second.c_str());
			if (pr.first == "dropProb")
				dropProb = atof(pr.second.c_str());
			if (pr.first == "delta")
				delta = atof(pr.second.c_str());
			if (pr.first == "beam")
				beam = atoi(pr.second.c_str());

			if (pr.first == "stateHiddenSize")
				state_hiddensize = atoi(pr.second.c_str());

			if (pr.first == "wordEmbSize")
				wordEmbSize = atoi(pr.second.c_str());
			if (pr.first == "wordHiddenSize")
				wordHiddenSize = atoi(pr.second.c_str());
			if (pr.first == "wordRNNHiddenSize")
				wordRNNHiddenSize = atoi(pr.second.c_str());
			if (pr.first == "wordEmbFineTune")
				wordEmbFineTune = (pr.second == "true") ? true : false;
			if (pr.first == "wordEmbFile")
				wordEmbFile = pr.second;
			if (pr.first == "wordcontext")
				wordcontext = atoi(pr.second.c_str());

			if (pr.first == "mapFile")
				mapFile = pr.second;
			if (pr.first == "lmFile")
				lmFile = pr.second;

			if (pr.first == "actionEmbSize")
				actionEmbSize = atoi(pr.second.c_str());
			if (pr.first == "actionHiddenSize")
				actionHiddenSize = atoi(pr.second.c_str());
			if (pr.first == "actionRNNHiddenSize")
				actionRNNHiddenSize = atoi(pr.second.c_str());

			if (pr.first == "verboseIter")
				verboseIter = atoi(pr.second.c_str());
			if (pr.first == "train")
				train = (pr.second == "true") ? true : false;
			if (pr.first == "maxInstance")
				maxInstance = atoi(pr.second.c_str());
			if (pr.first == "testFile")
				testFiles.push_back(pr.second);
			if (pr.first == "outBest")
				outBest = pr.second;

			if (pr.first == "maxLength")
				maxLength = atoi(pr.second.c_str());
			if (pr.first == "maxCandidAction")
				maxCandidAction = atoi(pr.second.c_str());
			if (pr.first == "unkStrategy")
				unkStrategy = atoi(pr.second.c_str());

			if (pr.first == "labelFeatEmbSize")
				labelFeatEmbSize = atoi(pr.second.c_str());
			if (pr.first == "labelFeatEmbFineTune")
				labelFeatEmbFineTune = (pr.second == "true") ? true : false;

		}
	}

	void showOptions() {
		std::cout << "wordCutOff = " << wordCutOff << std::endl;
		std::cout << "featCutOff = " << featCutOff << std::endl;
		std::cout << "maxIter = " << maxIter << std::endl;
		std::cout << "batchSize = " << batchSize << std::endl;
		std::cout << "adaEps = " << adaEps << std::endl;
		std::cout << "adaAlpha = " << adaAlpha << std::endl;
		std::cout << "regParameter = " << regParameter << std::endl;
		std::cout << "dropProb = " << dropProb << std::endl;
		std::cout << "delta = " << delta << std::endl;
		std::cout << "beam = " << beam << std::endl;
		std::cout << "maxLength = " << maxLength << std::endl;
		std::cout << "maxCandidAction = " << maxCandidAction << std::endl;
		std::cout << "unkStrategy = " << unkStrategy << std::endl;

		std::cout << "train = " << train << std::endl;
		std::cout << "verboseIter = " << verboseIter << std::endl;
		std::cout << "maxInstance = " << maxInstance << std::endl;
		std::cout << "outBest = " << outBest << std::endl;

		std::cout << "wordEmbSize = " << wordEmbSize << std::endl;
		std::cout << "wordEmbFineTune = " << wordEmbFineTune << std::endl;
		std::cout << "wordcontext = " << wordcontext << std::endl;
		std::cout << "wordHiddenSize = " << wordHiddenSize << std::endl;
		std::cout << "wordRNNHiddenSize = " << wordRNNHiddenSize << std::endl;


		std::cout << "actionEmbSize = " << actionEmbSize << std::endl;
		std::cout << "actionHiddenSize = " << actionHiddenSize << std::endl;
		std::cout << "actionRNNHiddenSize = " << actionRNNHiddenSize << std::endl;

		std::cout << "stateHiddenSize = " << state_hiddensize << std::endl;

		std::cout << "labelFeatEmbSize = " << labelFeatEmbSize << std::endl;
		std::cout << "labelFeatEmbFineTune = " << labelFeatEmbFineTune << std::endl;

		std::cout << "mapFile = " << mapFile << std::endl;
		std::cout << "lmFile = " << lmFile << std::endl;
		std::cout << "wordEmbFile = " << wordEmbFile << std::endl;
		for (int idx = 0; idx < testFiles.size(); idx++) {
			std::cout << "testFile = " << testFiles[idx] << std::endl;
		}
		std::cout << endl;
	}

	void load(const std::string& infile) {
		ifstream inf;
		inf.open(infile.c_str());
		vector<string> vecLine;
		while (1) {
			string strLine;
			if (!my_getline(inf, strLine)) {
				break;
			}
			if (strLine.empty())
				continue;
			vecLine.push_back(strLine);
		}
		inf.close();
		setOptions(vecLine);
	}
};

#endif

