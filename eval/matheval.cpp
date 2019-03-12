#include "matheval.h"

double evaluate(std::string expr, std::map<std::string, double> variables) {
    /*double value;
    std::istringstream is(expr);
    if(is >> value) return value;
    return variables[expr];*/

    /*exprtk::symbol_table<double> symbol_table;
    for(auto it = variables.begin(); it!=variables.end(); ++it)
        symbol_table.add_constant(it->first,it->second);

    //symbol_table.add_variable("x",x);
    //symbol_table.add_constants();

    exprtk::expression<double> expression;
    expression.register_symbol_table(symbol_table);

    exprtk::parser<double> parser;
    parser.compile(expr,expression);

    return expression.value();*/

    //Parser p (expr);
    //p.Evaluate()
    //p.symbols_ = variables;
    //return p.Evaluate();

}
