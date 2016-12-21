#ifndef _CONLL_READER_
#define _CONLL_READER_

#include "Reader.h"
#include "N3L.h"
#include "Utf.h"
#include <sstream>

using namespace std;

class InstanceReader : public Reader {
public:
	InstanceReader() {
	}
	~InstanceReader() {
	}

	Instance *getNext() {
		m_instance.clear();
		static string strLine;
		static vector<string> vecLine;
		vecLine.clear();
		while (1) {
			if (!my_getline(m_inf, strLine)) {
				break;
			}
			if (strLine.empty())
				break;
			vecLine.push_back(strLine);
		}

		if (vecLine.size() == 0) {
			return NULL;
		}

		if (vecLine.size() != 2) {
			std::cout << "error: each instance should have two lines!" << std::endl;
			return getNext();
		}

		vector<string> wordInfo;
		vector<string> normwordInfo;		
		vector<string> charInfo;

		split_bychar(vecLine[0], wordInfo, ' ');
		split_bychar(vecLine[1], normwordInfo, ' ');

		if (wordInfo.size() != normwordInfo.size()) {
			std::cout << "error: word and normword sequence not match!" << std::endl;
			return getNext();
		}

		int charcount = 0;
		strLine = "";
		for (int i = 0; i < wordInfo.size(); ++i) {
			charcount += getUTF8StringLength(wordInfo[i]);
			strLine += wordInfo[i];
		}

		getCharactersFromUTF8String(strLine, charInfo);

		if (charcount != charInfo.size()) {
			std::cout << "error: seg and char sequence not match!" << std::endl;
			return getNext();
		}

		m_instance.allocate(wordInfo.size(), charInfo.size());
		for (int i = 0; i < wordInfo.size(); ++i) {
			m_instance.words[i] = wordInfo[i];
			m_instance.normwords[i] = normwordInfo[i];
		}
		for (int i = 0; i < charInfo.size(); ++i) {
			m_instance.chars[i] = charInfo[i];
		}

		return &m_instance;
	}
};

#endif

