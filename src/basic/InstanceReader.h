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

		int offset = 0;
		if (postwordInfo[0].find("[PO]POLAR#") != std::string::npos){
			m_instance.emotion = postwordInfo[0];
			m_instance.stance = postwordInfo[1];
			offset = 2;
		}

		m_instance.allocate(postwordInfo.size() - offset, responsewordInfo.size(), charInfo.size());
		for (int i = offset; i < postwordInfo.size() ; ++i) {
			m_instance.post_words[i - offset] = postwordInfo[i];
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

