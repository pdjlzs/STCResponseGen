/*
 * RespondGen.cpp
 *
 *  Created on: Jan 25, 2016
 *      Author: mszhang
 */

#include "NNGreedyResponseGen.h"

#include "Argument_helper.h"

RespondGen::RespondGen(size_t memsize) : m_driver(memsize) {
	// TODO Auto-generated constructor stub
	srand(0);
	//Node::id = 0;
}

RespondGen::~RespondGen() {
	// TODO Auto-generated destructor stub
}

int RespondGen::initialActionWordMap() {
	cout << "Creating candidate action alphabet..." << endl;
	m_driver._hyperparams.word_map.clear();
	//m_driver._hyperparams.reverse_word_map.clear();
	if (m_options.mapFile == "") {
		return -1;
	}

	static ifstream inf;
	if (inf.is_open()) {
		inf.close();
		inf.clear();
	}
	inf.open(m_options.mapFile.c_str());
	int maxCandidate = 0;
	int dic_num = 0;
	static string strLine;
	static vector<string> vecInfo;
	while (1) {
		if (!my_getline(inf, strLine)) {
			break;
		}
		if (!strLine.empty()) {
			split_bychar(strLine, vecInfo, ' ');
			for (int j = 1; j < vecInfo.size(); j++) {
				m_driver._hyperparams.word_map[vecInfo[0]].push_back(vecInfo[j]);
			}
			if (vecInfo.size() - 1 > maxCandidate) maxCandidate = vecInfo.size() - 1;
		}
		if ((dic_num + 1) % m_options.verboseIter == 0) {
			cout << dic_num + 1 << " ";
			if ((dic_num + 1) % (40 * m_options.verboseIter) == 0)
				cout << std::endl;
			cout.flush();
		}
		dic_num++;
	}
	cout << std::endl;
	inf.close();
	m_driver._hyperparams.action_num = CAction::NO_ACTION + maxCandidate;
}

// all linear features are extracted from positive examples
int RespondGen::createAlphabet(const vector<Instance>& vecInsts) {
	cout << "Creating Alphabet..." << endl;

	int numInstance = vecInsts.size();

	assert(numInstance > 0);
	int count = 0;
	for (numInstance = 0; numInstance < vecInsts.size(); numInstance++) {
		const Instance &instance = vecInsts[numInstance];
		for (int idx = 0; idx < instance.postWordsize(); idx++) {
			m_driver._hyperparams.word_stat[normalize_to_lowerwithdigit(instance.post_words[idx])]++;
		}
	}
	m_driver._hyperparams.word_stat[nullkey] = m_options.wordCutOff + 1;
	m_driver._hyperparams.word_stat[unknownkey] = m_options.wordCutOff + 1;

	if (!m_options.wordEmbFineTune && m_options.wordEmbFile != "") {
		m_driver._modelparams.word_ext_alphas.initial(m_options.wordEmbFile);
	}
	if (!m_driver._modelparams.word_ext_alphas.is_fixed()) {
		m_driver._modelparams.word_ext_alphas.initial(m_driver._hyperparams.word_stat, m_options.wordCutOff);
	}

	m_driver._modelparams.word_alpha.initial(m_driver._hyperparams.word_stat, m_options.wordCutOff);
	unordered_map<string, int> action_stat;

	vector<CStateItem> state(m_driver._hyperparams.maxlength + 1);
	vector<string> respon_output;
	CAction answer;
	Metric eval;
	int actionNum;
	eval.reset();
	for (numInstance = 0; numInstance < vecInsts.size(); numInstance++) {
		const Instance &instance = vecInsts[numInstance];
		actionNum = 0;
		state[actionNum].clear();
		action_stat[state[actionNum]._lastAction.str()]++;
		while (!state[actionNum].IsTerminated()) {
			state[actionNum].getGoldAction(instance.respon_words, answer);
			state[actionNum].move(&(state[actionNum + 1]), answer);
			actionNum++;
			action_stat[answer.str()]++;
		}

		state[actionNum].getSegResults(respon_output);

		instance.evaluate(respon_output, eval);

		if (!eval.bIdentical()) {
			std::cout << "error state conversion!" << std::endl;
			exit(0);
		}

		if ((numInstance + 1) % m_options.verboseIter == 0) {
			cout << numInstance + 1 << " ";
			if ((numInstance + 1) % (40 * m_options.verboseIter) == 0)
				cout << std::endl;
			cout.flush();
		}
		if (m_options.maxInstance > 0 && numInstance == m_options.maxInstance)
			break;
	}

	action_stat[nullkey] = 1;
	action_stat[unknownkey] = 1;
	m_driver._modelparams.action_alpha.initial(action_stat, 0);

	cout << numInstance << " " << endl;
	cout << "Train file total word num: " << m_driver._hyperparams.word_stat.size() << endl;
	cout << "Fine tune embedding alphabet remain word num: " << m_driver._modelparams.word_alpha.size() << endl;
	cout << "External embedding alphabet remain word num: " << m_driver._modelparams.word_ext_alphas.size() << endl;

	return 0;
}

void RespondGen::getGoldActions(const vector<Instance>& vecInsts, vector<vector<CAction> >& vecActions) {
	std::cout << "Get gold actions for traing ..." << endl;
	vecActions.clear();

	Metric eval;
	vector<CStateItem> state(m_driver._hyperparams.maxlength + 1);
	vector<string> respon_output;
	CAction answer;
	eval.reset();
	static int numInstance, actionNum;
	vecActions.resize(vecInsts.size());
	for (numInstance = 0; numInstance < vecInsts.size(); numInstance++) {
		const Instance &instance = vecInsts[numInstance];

		actionNum = 0;
		state[actionNum].clear();
		while (!state[actionNum].IsTerminated()) {
			state[actionNum].getGoldAction(instance.respon_words, answer);
			vecActions[numInstance].push_back(answer);
			state[actionNum].move(&state[actionNum + 1], answer);
			actionNum++;
		}

		state[actionNum].getSegResults(respon_output);

		instance.evaluate(respon_output, eval);

		if (!eval.bIdentical()) {
			std::cout << "error state conversion!" << std::endl;
			exit(0);
		}

		if ((numInstance + 1) % m_options.verboseIter == 0) {
			cout << numInstance + 1 << " ";
			if ((numInstance + 1) % (40 * m_options.verboseIter) == 0)
				cout << std::endl;
			cout.flush();
		}
		if (m_options.maxInstance > 0 && numInstance == m_options.maxInstance)
			break;
	}
	cout << endl;
}

void RespondGen::train(const string& trainFile, const string& devFile, const string& testFile, const string& modelFile, const string& optionFile) {
	if (optionFile != "")
		m_options.load(optionFile);

	m_options.showOptions();

	initialActionWordMap();

	m_driver._hyperparams.setRequared(m_options);

	vector<Instance> trainInsts, devInsts, testInsts;
	std::cout << "Loading train, dev, test corpus ... " << std::endl;
	m_pipe.readInstances(trainFile, trainInsts, m_driver._hyperparams.maxlength, m_options.maxInstance);
	if (devFile != "")
		m_pipe.readInstances(devFile, devInsts, m_driver._hyperparams.maxlength, m_options.maxInstance);
	if (testFile != "")
		m_pipe.readInstances(testFile, testInsts, m_driver._hyperparams.maxlength, m_options.maxInstance);

	vector<vector<Instance> > otherInsts(m_options.testFiles.size());
	for (int idx = 0; idx < m_options.testFiles.size(); idx++) {
		m_pipe.readInstances(m_options.testFiles[idx], otherInsts[idx], m_driver._hyperparams.maxlength, m_options.maxInstance);
	}

	createAlphabet(trainInsts);

	//lookup table setting
	bool initial_successed = false;
	if (m_options.wordEmbFile != "") {
		initial_successed = m_driver._modelparams.word_ext_table.initial(&m_driver._modelparams.word_ext_alphas, m_options.wordEmbFile, m_options.wordEmbFineTune);
		if (initial_successed) {
			m_options.wordEmbSize = m_driver._modelparams.word_ext_table.nDim;
		}
	}
	if (!initial_successed) {
		m_options.wordEmbFineTune = true;
		m_driver._modelparams.word_ext_table.initial(&m_driver._modelparams.word_ext_alphas, m_options.wordEmbSize, true);
	}

	m_driver._modelparams.word_table.initial(&m_driver._modelparams.word_alpha, m_options.wordEmbSize, true);
	m_driver._modelparams.action_table.initial(&m_driver._modelparams.action_alpha, m_options.actionEmbSize, true);
	m_driver.initial();

	vector<vector<CAction> > trainInstGoldactions;
	getGoldActions(trainInsts, trainInstGoldactions);
	double bestFmeasure = 0;

	int inputSize = trainInsts.size();

	std::vector<int> indexes;
	for (int i = 0; i < inputSize; ++i)
		indexes.push_back(i);

	static Metric eval_train, eval_dev, eval_test;

	//int maxIter = m_options.maxIter * (inputSize / m_options.batchSize + 1);
	int maxIter = m_options.maxIter;
	//int oneIterMaxRound = (inputSize + m_options.batchSize - 1) / m_options.batchSize;
	std::cout << "maxIter = " << maxIter << std::endl;
	int devNum = devInsts.size(), testNum = testInsts.size();

	static vector<vector<string> > decodeInstResults;
	static vector<string> curDecodeInst;
	static bool bCurIterBetter;
	static vector<vector<string> > subInstances;
	static vector<vector<CAction> > subInstGoldActions;
	static Instance inst;
	static vector<CAction> actions;

	for (int iter = 0; iter < maxIter; ++iter) {
		std::cout << "##### Iteration " << iter << std::endl;
		srand(iter);
		random_shuffle(indexes.begin(), indexes.end());
		std::cout << "random: " << indexes[0] << ", " << indexes[indexes.size() - 1] << std::endl;
		bool bEvaluate = false;

		if (m_options.batchSize == 1) {
			eval_train.reset();
			bEvaluate = true;
			for (int idy = 0; idy < inputSize; idy++) {
				subInstances.clear();
				subInstGoldActions.clear();
				subInstances.push_back(trainInsts[indexes[idy]].post_words);
				subInstGoldActions.push_back(trainInstGoldactions[indexes[idy]]);

				double cost = m_driver.train(subInstances, subInstGoldActions);

				eval_train.overall_label_count += m_driver._eval.overall_label_count;
				eval_train.correct_label_count += m_driver._eval.correct_label_count;

				if ((idy + 1) % (m_options.verboseIter * 10) == 0) {
					std::cout << "current: " << idy + 1 << ", Cost = " << cost << ", Correct(%) = " << eval_train.getAccuracy() << std::endl;
				}
				m_driver.updateModel();
			}
			std::cout << "current: " << iter + 1 << ", Correct(%) = " << eval_train.getAccuracy() << std::endl;
		}
		else {
			if (iter == 0)eval_train.reset();
			subInstances.clear();
			subInstGoldActions.clear();
			for (int idy = 0; idy < m_options.batchSize; idy++) {
				subInstances.push_back(trainInsts[indexes[idy]].post_words);
				subInstGoldActions.push_back(trainInstGoldactions[indexes[idy]]);

			}
			double cost = m_driver.train(subInstances, subInstGoldActions);

			eval_train.overall_label_count += m_driver._eval.overall_label_count;
			eval_train.correct_label_count += m_driver._eval.correct_label_count;

			if ((iter + 1) % (m_options.verboseIter) == 0) {
				std::cout << "current: " << iter + 1 << ", Cost = " << cost << ", Correct(%) = " << eval_train.getAccuracy() << std::endl;
				eval_train.reset();
				bEvaluate = true;
			}

			m_driver.updateModel();
		}

		if (bEvaluate && devNum > 0) {
			clock_t time_start = clock();
			std::cout << "Dev start." << std::endl;
			bCurIterBetter = false;
			if (!m_options.outBest.empty()) {
				decodeInstResults.clear();
			}
			eval_dev.reset();
			for (int idx = 0; idx < devInsts.size(); idx++) {
				predict(devInsts[idx], curDecodeInst);
				devInsts[idx].evaluate(curDecodeInst, eval_dev);
				if (!m_options.outBest.empty()) {
					decodeInstResults.push_back(curDecodeInst);
				}
			}
			std::cout << "Dev finished. Total time taken is: " << double(clock() - time_start) / CLOCKS_PER_SEC << std::endl;
			std::cout << "dev:" << std::endl;
			eval_dev.print();

			if (!m_options.outBest.empty() && eval_dev.getAccuracy() > bestFmeasure) {
				m_pipe.outputAllInstances(devFile + m_options.outBest, devInsts, decodeInstResults);
				bCurIterBetter = true;
			}

			if (testNum > 0) {
				time_start = clock();
				std::cout << "Test start." << std::endl;
				if (!m_options.outBest.empty()) {
					decodeInstResults.clear();
				}
				eval_test.reset();
				for (int idx = 0; idx < testInsts.size(); idx++) {
					predict(testInsts[idx], curDecodeInst);
					testInsts[idx].evaluate(curDecodeInst, eval_test);
					if (bCurIterBetter && !m_options.outBest.empty()) {
						decodeInstResults.push_back(curDecodeInst);
					}
				}
				std::cout << "Test finished. Total time taken is: " << double(clock() - time_start) / CLOCKS_PER_SEC << std::endl;
				std::cout << "test:" << std::endl;
				eval_test.print();

				if (!m_options.outBest.empty() && bCurIterBetter) {
					m_pipe.outputAllInstances(testFile + m_options.outBest, testInsts, decodeInstResults);
				}
			}

			for (int idx = 0; idx < otherInsts.size(); idx++) {
				time_start = clock();
				std::cout << "processing " << m_options.testFiles[idx] << std::endl;
				if (!m_options.outBest.empty()) {
					decodeInstResults.clear();
				}
				eval_test.reset();
				for (int idy = 0; idy < otherInsts[idx].size(); idy++) {
					predict(otherInsts[idx][idy], curDecodeInst);
					otherInsts[idx][idy].evaluate(curDecodeInst, eval_test);
					if (bCurIterBetter && !m_options.outBest.empty()) {
						decodeInstResults.push_back(curDecodeInst);
						decodeInstResults.push_back(curDecodeInst);
					}
				}
				std::cout << "Test finished. Total time taken is: " << double(clock() - time_start) / CLOCKS_PER_SEC << std::endl;
				std::cout << "test:" << std::endl;
				eval_test.print();

				if (!m_options.outBest.empty() && bCurIterBetter) {
					m_pipe.outputAllInstances(m_options.testFiles[idx] + m_options.outBest, otherInsts[idx], decodeInstResults);
				}
			}


			if (eval_dev.getAccuracy() > bestFmeasure) {
				std::cout << "Exceeds best previous DIS of " << bestFmeasure << ". Saving model file.." << std::endl;
				bestFmeasure = eval_dev.getAccuracy();
				writeModelFile(modelFile);
			}
		}
	}
}

void RespondGen::predict(const Instance& input, vector<string>& resp_out) {
	m_driver.decode(input.post_words, resp_out);
}

void RespondGen::test(const string& testFile, const string& outputFile, const string& modelFile) {
	loadModelFile(modelFile);
	vector<Instance> testInsts;
	m_pipe.readInstances(testFile, testInsts, m_options.maxInstance);

	vector<vector<string> > testInstResults(testInsts.size()), testInstNormResults(testInsts.size());
	Metric eval_test;
	eval_test.reset();
	for (int idx = 0; idx < testInsts.size(); idx++) {
		predict(testInsts[idx], testInstNormResults[idx]);
		testInsts[idx].evaluate(testInstNormResults[idx], eval_test);
	}
	std::cout << "test:" << std::endl;
	eval_test.print();

	std::ofstream os(outputFile.c_str());

	for (int idx = 0; idx < testInsts.size(); idx++) {
		for (int idy = 0; idy < testInstResults[idx].size(); idy++) {
			os << testInstResults[idx][idy] << " ";
		}
		os << std::endl;
		for (int idy = 0; idy < testInstResults[idx].size(); idy++) {
			os << testInstNormResults[idx][idy] << " ";
		}
		os << std::endl;
		os << std::endl;
	}
	os.close();
}


void RespondGen::loadModelFile(const string& inputModelFile) {

}

void RespondGen::writeModelFile(const string& outputModelFile) {

}

int main(int argc, char* argv[]) {
	std::string trainFile = "", devFile = "", testFile = "", modelFile = "";
	std::string wordEmbFile = "", optionFile = "";
	std::string outputFile = "";
	bool bTrain = false;
	dsr::Argument_helper ah;
	int memsize = 1;


	ah.new_flag("l", "learn", "train or test", bTrain);
	ah.new_named_string("train", "trainCorpus", "named_string", "training corpus to train a model, must when training", trainFile);
	ah.new_named_string("dev", "devCorpus", "named_string", "development corpus to train a model, optional when training", devFile);
	ah.new_named_string("test", "testCorpus", "named_string",
		"testing corpus to train a model or input file to test a model, optional when training and must when testing", testFile);
	ah.new_named_string("model", "modelFile", "named_string", "model file, must when training and testing", modelFile);
	ah.new_named_string("word", "wordEmbFile", "named_string", "pretrained word embedding file to train a model, optional when training", wordEmbFile);
	ah.new_named_string("option", "optionFile", "named_string", "option file to train a model, optional when training", optionFile);
	ah.new_named_string("output", "outputFile", "named_string", "output file to test, must when testing", outputFile);
	ah.new_named_int("mem", "memsize", "named_int", "memory allocated for tensor nodes", memsize);

	ah.process(argc, argv);

	RespondGen respondGen(memsize);
	if (bTrain) {
		respondGen.train(trainFile, devFile, testFile, modelFile, optionFile);
	}
	else {
		respondGen.test(testFile, outputFile, modelFile);
	}

	//test(argv);
	//ah.write_values(std::cout);

}
