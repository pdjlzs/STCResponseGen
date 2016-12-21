/*
 * Normalizer.cpp
 *
 *  Created on: Jan 25, 2016
 *      Author: mszhang
 */

#include "NNRespondGen.h"

#include "Argument_helper.h"

Normalizer::Normalizer(size_t memsize) : m_driver(memsize) {
	// TODO Auto-generated constructor stub
	srand(0);
	//Node::id = 0;
}

Normalizer::~Normalizer() {
	// TODO Auto-generated destructor stub
}

int Normalizer::initialActionWordMap() {
	m_driver._hyperparams.word_map.clear();
	m_driver._hyperparams.reverse_word_map.clear();
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
				m_driver._hyperparams.reverse_word_map[vecInfo[j]].push_back(vecInfo[0]);
			}
			if (vecInfo.size() - 1 > maxCandidate) maxCandidate = vecInfo.size() - 1;
		}
	}
	inf.close();
	m_driver._hyperparams.action_num = CAction::NO_ACTION + maxCandidate;
}

// all linear features are extracted from positive examples
int Normalizer::createAlphabet(const vector<Instance>& vecInsts) {
	cout << "Creating Alphabet..." << endl;

	int numInstance = vecInsts.size();

	unordered_map<string, int> word_stat;
	unordered_map<string, int> char_stat;
	unordered_map<string, int> bichar_stat;
	unordered_map<string, int> charType_stat;
	charType_stat["U"] = 1;
	charType_stat["u"] = 1;
	charType_stat["e"] = 1;
	charType_stat["E"] = 1;
	charType_stat["p"] = 1;
	charType_stat["d"] = 1;
	charType_stat["o"] = 1;

	assert(numInstance > 0);
	int count = 0;
	for (numInstance = 0; numInstance < vecInsts.size(); numInstance++) {
		const Instance &instance = vecInsts[numInstance];
		for (int idx = 0; idx < instance.wordsize(); idx++) {
			word_stat[normalize_to_lowerwithdigit(instance.words[idx])]++;
		}
		for (int idx = 0; idx < instance.charsize(); idx++) {
			charType_stat[instance.chars[idx]]++;
			char_stat[instance.chars[idx]]++;
			if (idx < instance.charsize() - 1) {
				bichar_stat[instance.chars[idx] + instance.chars[idx + 1]]++;
			}
		}
		count += instance.wordsize();
	}
	word_stat[nullkey] = m_options.wordCutOff + 1;
	word_stat[unknownkey] = m_options.wordCutOff + 1;
	char_stat[nullkey] = m_options.charCutOff + 1;
	char_stat[unknownkey] = m_options.charCutOff + 1;
	bichar_stat[nullkey] = m_options.bicharCutOff + 1;
	bichar_stat[unknownkey] = m_options.bicharCutOff + 1;
	m_driver._modelparams.words.initial(word_stat, 0);
	m_driver._modelparams.chars.initial(char_stat, 0);

	if (!m_options.wordEmbFineTune && m_options.wordEmbFile != "") {
		m_driver._modelparams.embeded_words.initial(m_options.wordEmbFile);
	}
	if (!m_driver._modelparams.embeded_words.is_fixed()) {
		m_driver._modelparams.embeded_words.initial(word_stat, m_options.wordCutOff);
	}

	if (!m_options.charEmbFineTune && m_options.charEmbFile != "") {
		m_driver._modelparams.embeded_chars.initial(m_options.charEmbFile);
	}
	if (!m_driver._modelparams.embeded_chars.is_fixed()) {
		m_driver._modelparams.embeded_chars.initial(char_stat, m_options.charCutOff);
	}

	if (!m_options.bicharEmbFineTune && m_options.bicharEmbFile != "") {
		m_driver._modelparams.embeded_bichars.initial(m_options.bicharEmbFile);
	}
	if (!m_driver._modelparams.embeded_bichars.is_fixed()) {
		m_driver._modelparams.embeded_bichars.initial(bichar_stat, m_options.charCutOff);
	}

	m_driver._modelparams.embeded_chartypes.initial(charType_stat);
	charType_stat[nullkey] = 1;
	m_driver._modelparams.charTypes.initial(charType_stat);


	unordered_map<string, int> action_stat;

	vector<CStateItem> state(m_driver._hyperparams.maxlength + 1);
	vector<string> output, normoutput;
	CAction answer;
	Metric eval, evalnorm;
	int actionNum;
	eval.reset(); evalnorm.reset();
	for (numInstance = 0; numInstance < vecInsts.size(); numInstance++) {
		const Instance &instance = vecInsts[numInstance];
		actionNum = 0;
		state[actionNum].clear();
		state[actionNum].setInput(&instance.chars);
		action_stat[state[actionNum]._lastAction.typestr()]++;
		while (!state[actionNum].IsTerminated()) {
			state[actionNum].getGoldAction(instance.words, instance.normwords, answer);
			state[actionNum].move(&(state[actionNum + 1]), answer);
			actionNum++;
			action_stat[answer.typestr()]++;
		}

		if (actionNum - 1 != instance.charsize()) {
			std::cout << "action number is not correct, please check" << std::endl;
		}
		state[actionNum].getSegResults(output, normoutput);

		instance.evaluate(output, normoutput, eval, evalnorm);

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
	m_driver._modelparams.embeded_actions.initial(action_stat, 0);

	cout << numInstance << " " << endl;
	cout << "Total word num: " << word_stat.size() << endl;
	cout << "Total char num: " << char_stat.size() << endl;

	cout << "Remain word num: " << m_driver._modelparams.words.size() << endl;
	cout << "Remain char num: " << m_driver._modelparams.chars.size() << endl;
	cout << "Remain charType num: " << m_driver._modelparams.charTypes.size() << endl;

	return 0;
}

void Normalizer::getGoldActions(const vector<Instance>& vecInsts, vector<vector<CAction> >& vecActions) {
	vecActions.clear();

	Metric eval, evalnorm;
	vector<CStateItem> state(m_driver._hyperparams.maxlength + 1);
	vector<string> output, normoutput;
	CAction answer;
	eval.reset(); evalnorm.reset();
	static int numInstance, actionNum;
	vecActions.resize(vecInsts.size());
	for (numInstance = 0; numInstance < vecInsts.size(); numInstance++) {
		const Instance &instance = vecInsts[numInstance];

		actionNum = 0;
		state[actionNum].clear();
		state[actionNum].setInput(&instance.chars);
		while (!state[actionNum].IsTerminated()) {
			state[actionNum].getGoldAction(instance.words, instance.normwords, answer);
			vecActions[numInstance].push_back(answer);
			state[actionNum].move(&state[actionNum + 1], answer);
			actionNum++;
		}

		if (actionNum - 1 != instance.charsize()) {
			std::cout << "action number is not correct, please check" << std::endl;
		}
		state[actionNum].getSegResults(output, normoutput);

		instance.evaluate(output, normoutput, eval, evalnorm);

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
}

void Normalizer::getGoldActions(const Instance& inst, vector<CAction>& actions) {
	actions.clear();

	Metric eval, evalnorm;
	vector<CStateItem> state(m_driver._hyperparams.maxlength + 1);
	vector<string> output, normoutput;
	CAction answer;
	eval.reset(); evalnorm.reset();
	int actionNum = 0;
	state[actionNum].clear();
	state[actionNum].setInput(&inst.chars);
	while (!state[actionNum].IsTerminated()) {
		state[actionNum].getGoldAction(inst.words, inst.normwords, answer);
		actions.push_back(answer);
		state[actionNum].move(&state[actionNum + 1], answer);
		actionNum++;
	}

	if (actionNum - 1 != inst.charsize()) {
		std::cout << "action number is not correct, please check" << std::endl;
	}
	state[actionNum].getSegResults(output, normoutput);

	inst.evaluate(output, normoutput, eval, evalnorm);

	if (!eval.bIdentical()) {
		std::cout << "error state conversion!" << std::endl;
		exit(0);
	}
}

void Normalizer::train(const string& trainFile, const string& devFile, const string& testFile, const string& modelFile, const string& optionFile) {
	if (optionFile != "")
		m_options.load(optionFile);

	m_options.showOptions();

	initialActionWordMap();

	vector<Instance> trainInsts, devInsts, testInsts;
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
		initial_successed = m_driver._modelparams.word_table.initial(&m_driver._modelparams.embeded_words, m_options.wordEmbFile, m_options.wordEmbFineTune, m_options.wordEmbNormalize);
		if (initial_successed) {
			m_options.wordEmbSize = m_driver._modelparams.word_table.nDim;
		}
	}
	if (!initial_successed) {
		m_options.wordEmbFineTune = true;
		m_driver._modelparams.word_table.initial(&m_driver._modelparams.embeded_words, m_options.wordEmbSize, true);
	}

	initial_successed = false;
	if (m_options.charEmbFile != "") {
		initial_successed = m_driver._modelparams.char_table.initial(&m_driver._modelparams.embeded_chars, m_options.charEmbFile, m_options.charEmbFineTune, m_options.charEmbNormalize);
		if (initial_successed) {
			m_options.charEmbSize = m_driver._modelparams.char_table.nDim;
		}
	}
	if (!initial_successed) {
		m_options.charEmbFineTune = true;
		m_driver._modelparams.char_table.initial(&m_driver._modelparams.embeded_chars, m_options.charEmbSize, true);
	}

	initial_successed = false;
	if (m_options.bicharEmbFile != "") {
		initial_successed = m_driver._modelparams.bichar_table.initial(&m_driver._modelparams.embeded_bichars, m_options.bicharEmbFile, m_options.bicharEmbFineTune, m_options.bicharEmbNormalize);
		if (initial_successed) {
			m_options.bicharEmbSize = m_driver._modelparams.bichar_table.nDim;
		}
	}
	if (!initial_successed) {
		m_options.bicharEmbFineTune = true;
		m_driver._modelparams.bichar_table.initial(&m_driver._modelparams.embeded_bichars, m_options.bicharEmbSize, true);
	}

	m_driver._modelparams.action_table.initial(&m_driver._modelparams.embeded_actions, m_options.actionEmbSize, true);
	m_driver._modelparams.chartype_table.initial(&m_driver._modelparams.embeded_chartypes, m_options.actionEmbSize, true);

	//m_driver._hyperparams.action_num = CAction::FIN + 1;
	m_driver._hyperparams.setRequared(m_options);
	m_driver.initial();

	vector<vector<CAction> > trainInstGoldactions;
	getGoldActions(trainInsts, trainInstGoldactions);
	double bestFmeasure = 0;

	int inputSize = trainInsts.size();

	std::vector<int> indexes;
	for (int i = 0; i < inputSize; ++i)
		indexes.push_back(i);

	static Metric eval, metric_dev, metric_test, metricnorm_dev, metricnorm_test;

	int maxIter = m_options.maxIter * (inputSize / m_options.batchSize + 1);
	int oneIterMaxRound = (inputSize + m_options.batchSize - 1) / m_options.batchSize;
	std::cout << "maxIter = " << maxIter << std::endl;
	int devNum = devInsts.size(), testNum = testInsts.size();

	static vector<vector<string> > decodeInstResults, decodeInstNormResults;
	static vector<string> curDecodeInst, curDecodeNormInst;
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
			eval.reset();
			bEvaluate = true;
			for (int idy = 0; idy < inputSize; idy++) {
				subInstances.clear();
				subInstGoldActions.clear();
				randomInput(trainInsts[indexes[idy]], inst);
				subInstances.push_back(inst.chars);
				getGoldActions(inst, actions);
				subInstGoldActions.push_back(actions);

				double cost = m_driver.train(subInstances, subInstGoldActions);

				eval.overall_label_count += m_driver._eval.overall_label_count;
				eval.correct_label_count += m_driver._eval.correct_label_count;

				if ((idy + 1) % (m_options.verboseIter * 10) == 0) {
					std::cout << "current: " << idy + 1 << ", Cost = " << cost << ", Correct(%) = " << eval.getAccuracy() << std::endl;
				}
				m_driver.updateModel();
			}
			std::cout << "current: " << iter + 1 << ", Correct(%) = " << eval.getAccuracy() << std::endl;
		}
		else {
			if (iter == 0)eval.reset();
			subInstances.clear();
			subInstGoldActions.clear();
			for (int idy = 0; idy < m_options.batchSize; idy++) {
				randomInput(trainInsts[indexes[idy]], inst);
				subInstances.push_back(inst.chars);
				getGoldActions(inst, actions);
				subInstGoldActions.push_back(actions);
			}
			double cost = m_driver.train(subInstances, subInstGoldActions);

			eval.overall_label_count += m_driver._eval.overall_label_count;
			eval.correct_label_count += m_driver._eval.correct_label_count;

			if ((iter + 1) % (m_options.verboseIter) == 0) {
				std::cout << "current: " << iter + 1 << ", Cost = " << cost << ", Correct(%) = " << eval.getAccuracy() << std::endl;
				eval.reset();
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
				decodeInstNormResults.clear();
			}
			metric_dev.reset(); metricnorm_dev.reset();
			for (int idx = 0; idx < devInsts.size(); idx++) {
				predict(devInsts[idx], curDecodeInst, curDecodeNormInst);
				devInsts[idx].evaluate(curDecodeInst, curDecodeNormInst, metric_dev, metricnorm_dev);
				if (!m_options.outBest.empty()) {
					decodeInstResults.push_back(curDecodeInst);
					decodeInstNormResults.push_back(curDecodeNormInst);
				}
			}
			std::cout << "Dev finished. Total time taken is: " << double(clock() - time_start) / CLOCKS_PER_SEC << std::endl;
			std::cout << "dev:" << std::endl;
			metric_dev.print();
			metricnorm_dev.print();

			if (!m_options.outBest.empty() && metric_dev.getAccuracy() > bestFmeasure) {
				m_pipe.outputAllInstances(devFile + m_options.outBest, devInsts, decodeInstResults, decodeInstNormResults);
				bCurIterBetter = true;
			}

			if (testNum > 0) {
				time_start = clock();
				std::cout << "Test start." << std::endl;
				if (!m_options.outBest.empty()) {
					decodeInstResults.clear();
					decodeInstNormResults.clear();
				}
				metric_test.reset(); metricnorm_test.reset();
				for (int idx = 0; idx < testInsts.size(); idx++) {
					predict(testInsts[idx], curDecodeInst, curDecodeNormInst);
					testInsts[idx].evaluate(curDecodeInst, curDecodeNormInst, metric_test, metricnorm_test);
					if (bCurIterBetter && !m_options.outBest.empty()) {
						decodeInstResults.push_back(curDecodeInst);
						decodeInstNormResults.push_back(curDecodeNormInst);
					}
				}
				std::cout << "Test finished. Total time taken is: " << double(clock() - time_start) / CLOCKS_PER_SEC << std::endl;
				std::cout << "test:" << std::endl;
				metric_test.print();
				metricnorm_test.print();

				if (!m_options.outBest.empty() && bCurIterBetter) {
					m_pipe.outputAllInstances(testFile + m_options.outBest, testInsts, decodeInstResults, decodeInstNormResults);
				}
			}

			for (int idx = 0; idx < otherInsts.size(); idx++) {
				time_start = clock();
				std::cout << "processing " << m_options.testFiles[idx] << std::endl;
				if (!m_options.outBest.empty()) {
					decodeInstResults.clear();
					decodeInstNormResults.clear();
				}
				metric_test.reset(); metricnorm_test.reset();
				for (int idy = 0; idy < otherInsts[idx].size(); idy++) {
					predict(otherInsts[idx][idy], curDecodeInst, curDecodeNormInst);
					otherInsts[idx][idy].evaluate(curDecodeInst, curDecodeNormInst, metric_test, metricnorm_test);
					if (bCurIterBetter && !m_options.outBest.empty()) {
						decodeInstResults.push_back(curDecodeInst);
						decodeInstNormResults.push_back(curDecodeNormInst);
					}
				}
				std::cout << "Test finished. Total time taken is: " << double(clock() - time_start) / CLOCKS_PER_SEC << std::endl;
				std::cout << "test:" << std::endl;
				metric_test.print();
				metricnorm_test.print();

				if (!m_options.outBest.empty() && bCurIterBetter) {
					m_pipe.outputAllInstances(m_options.testFiles[idx] + m_options.outBest, otherInsts[idx], decodeInstResults, decodeInstNormResults);
				}
			}


			if (m_options.saveIntermediate && metric_dev.getAccuracy() > bestFmeasure) {
				std::cout << "Exceeds best previous DIS of " << bestFmeasure << ". Saving model file.." << std::endl;
				bestFmeasure = metric_dev.getAccuracy();
				writeModelFile(modelFile);
			}
		}
	}
}

void Normalizer::predict(const Instance& input, vector<string>& output, vector<string>& normoutput) {
	m_driver.decode(input.chars, output, normoutput);
}

void Normalizer::test(const string& testFile, const string& outputFile, const string& modelFile) {
	loadModelFile(modelFile);
	vector<Instance> testInsts;
	m_pipe.readInstances(testFile, testInsts, m_options.maxInstance);

	vector<vector<string> > testInstResults(testInsts.size()), testInstNormResults(testInsts.size());
	Metric metric_test, metricnorm_test;
	metric_test.reset(); metricnorm_test.reset();
	for (int idx = 0; idx < testInsts.size(); idx++) {
		predict(testInsts[idx], testInstResults[idx], testInstNormResults[idx]);
		testInsts[idx].evaluate(testInstResults[idx], testInstNormResults[idx], metric_test, metricnorm_test);
	}
	std::cout << "test:" << std::endl;
	metric_test.print();
	metricnorm_test.print();

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


void Normalizer::loadModelFile(const string& inputModelFile) {

}

void Normalizer::writeModelFile(const string& outputModelFile) {

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

	Normalizer normalizer(memsize);
	if (bTrain) {
		normalizer.train(trainFile, devFile, testFile, modelFile, optionFile);
	}
	else {
		normalizer.test(testFile, outputFile, modelFile);
	}

	//test(argv);
	//ah.write_values(std::cout);

}
