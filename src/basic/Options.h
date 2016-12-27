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

	int sepHiddenSize;

	string wordEmbFile;
	int wordEmbSize;
	bool wordEmbFineTune;
	int wordHiddenSize;
	int wordRNNHiddenSize;

	int wordcontext;
	string mapFile;

	int actionEmbSize;
	int actionNgram;
	int actionHiddenSize;
	int actionRNNHiddenSize;

	int verboseIter;
	bool train;
	int maxInstance;
	vector<string> testFiles;
	string outBest;


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

		sepHiddenSize = 100;

		wordEmbSize = 50;
		wordHiddenSize = 150;
		wordRNNHiddenSize = 100;
		wordEmbFineTune = false;
		wordEmbFile = "";

		wordcontext = 2;
		mapFile = "";

		actionEmbSize = 20;
		actionNgram = 2;
		actionHiddenSize = 30;
		actionRNNHiddenSize = 20;

		verboseIter = 100;
		train = false;
		maxInstance = -1;
		testFiles.clear();
		outBest = "";
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

			if (pr.first == "sepHiddenSize")
				sepHiddenSize = atoi(pr.second.c_str());

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

			if (pr.first == "actionEmbSize")
				actionEmbSize = atoi(pr.second.c_str());
			if (pr.first == "actionNgram")
				actionNgram = atoi(pr.second.c_str());
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

		std::cout << "sepHiddenSize = " << sepHiddenSize << std::endl;

		std::cout << "wordEmbSize = " << wordEmbSize << std::endl;
		std::cout << "wordHiddenSize = " << wordHiddenSize << std::endl;
		std::cout << "wordRNNHiddenSize = " << wordRNNHiddenSize << std::endl;
		std::cout << "wordEmbFineTune = " << wordEmbFineTune << std::endl;
		std::cout << "wordEmbFile = " << wordEmbFile << std::endl;

		std::cout << "wordcontext = " << wordcontext << std::endl;

		std::cout << "mapFile = " << mapFile << std::endl;

		std::cout << "actionEmbSize = " << actionEmbSize << std::endl;
		std::cout << "actionNgram = " << actionNgram << std::endl;
		std::cout << "actionHiddenSize = " << actionHiddenSize << std::endl;
		std::cout << "actionRNNHiddenSize = " << actionRNNHiddenSize << std::endl;

		std::cout << "verboseIter = " << verboseIter << std::endl;
		std::cout << "train = " << train << std::endl;
		std::cout << "maxInstance = " << maxInstance << std::endl;
		for (int idx = 0; idx < testFiles.size(); idx++) {
			std::cout << "testFile = " << testFiles[idx] << std::endl;
		}
		std::cout << "outBest = " << outBest << std::endl;
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

