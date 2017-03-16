
#ifndef SRC_EVALUATION_H_
#define SRC_EVALUATION_H_

#include <math.h>
using namespace std;

struct Evaluation {

public:
  double sum_logprobabi;
  int word_num;
public:
  Evaluation()
  {
	sum_logprobabi = 0;
	word_num = 0;
  }

  ~Evaluation(){}

  void reset()
  {
	sum_logprobabi = 0;
	word_num = 0;
  }

  double getPerplexity()
  {
	  //return exp(-sum_logprobabi * log(10.0) / word_num);
	  return exp(-sum_logprobabi / word_num);
  }


  void print()
  {
	  //std::cout << "Perplexity\tP=" << "exp(-" << sum_logprobabi << "* log(10.0) /" << word_num << ")=" << getPerplexity() << std::endl;
	  std::cout << "Perplexity\tP=" << "exp(-" << sum_logprobabi << "/" << word_num << ")=" << getPerplexity() << std::endl;
  }


};

#endif /* SRC_EVALUATION_H_ */

