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

		vector<string> postwordInfo;
		vector<string> responsewordInfo;		
		vector<string> charInfo;

		split_bychar(vecLine[0], postwordInfo, ' ');
		split_bychar(vecLine[1], responsewordInfo, ' ');

		getCharactersFromUTF8String(vecLine[1], charInfo);

		m_instance.allocate(postwordInfo.size(), responsewordInfo.size(), charInfo.size());
		for (int i = 0; i < postwordInfo.size(); ++i) {
			m_instance.post_words[i] = postwordInfo[i];
		}
		for (int i = 0; i < responsewordInfo.size(); i++){
			m_instance.respon_words[i] = responsewordInfo[i];
		}
		for (int i = 0; i < charInfo.size(); ++i) {
			m_instance.respon_chars[i] = charInfo[i];
		}

		return &m_instance;
	}
};

#endif

