#ifndef MATHEVAL_H
#define MATHEVAL_H

//#include <core/exprtk.hpp>
#include <eval/parser.h>

extern double evaluate(std::string expr, std::map<std::string,double> variables);


#endif // MATHEVAL_H
