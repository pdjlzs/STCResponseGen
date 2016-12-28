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

	  for (int i = 0; i < pInstance->postWordsize(); ++i) {
		  m_outf << pInstance->post_words[i] << " ";
	  }
	  m_outf << endl;
	  for (int i = 0; i < pInstance->responWordsize(); ++i) {
	    m_outf << pInstance->respon_words[i] << " ";
	  }
	  m_outf << endl;
	  m_outf << endl;
	  return 0;
	}

  int write(const Instance *pInstance, const vector<string> &curWords)
  {
    if (!m_outf.is_open()) return -1;
    for (int i = 0; i < pInstance->postWordsize(); ++i) {
      m_outf << pInstance->post_words[i] << " ";
    }
    m_outf << endl;
	for (int i = 0; i < curWords.size(); ++i) {
		m_outf << curWords[i] << " ";
	}
	m_outf << endl;
	m_outf << endl;
    return 0;
  }
};

#endif

