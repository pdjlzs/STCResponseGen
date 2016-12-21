#ifndef _CONLL_WRITER_
#define _CONLL_WRITER_

#include "Writer.h"
#include <sstream>

using namespace std;

class InstanceWriter : public Writer
{
public:
	InstanceWriter(){}
	~InstanceWriter(){}
	int write(const Instance *pInstance)
	{
	  if (!m_outf.is_open()) return -1;

	  for (int i = 0; i < pInstance->wordsize(); ++i) {
		  m_outf << pInstance->words[i] << " ";
	  }
	  m_outf << endl;
	  for (int i = 0; i < pInstance->wordsize(); ++i) {
	    m_outf << pInstance->normwords[i] << " ";
	  }
	  m_outf << endl;
	  m_outf << endl;
	  return 0;
	}

  int write(const Instance *pInstance, const vector<string> &curWords, const vector<string> &curNormWords)
  {
    if (!m_outf.is_open()) return -1;
	for (int i = 0; i < curWords.size(); ++i) {
		m_outf << curWords[i] << " ";
	}
	m_outf << endl;
    for (int i = 0; i < curWords.size(); ++i) {
      m_outf << curNormWords[i] << " ";
    }
    m_outf << endl;
	m_outf << endl;
    return 0;
  }
};

#endif

