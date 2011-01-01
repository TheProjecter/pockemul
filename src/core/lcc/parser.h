#ifndef PARSER_H
#define PARSER_H

enum en_type
{
    DELIMITER,
    VARIABLE,
    NUMBER
};

class parser
{
    char *expression_ptr; /* Ptr points to expression */
    char token[50];
    char token_type;

    void evaluate_expression2(double &result);
    void evaluate_expression3(double &result);
    void evaluate_expression4(double &result);
    void evaluate_expression5(double &result);
    void evaluate_expression6(double &result);
    void atom(double &result);
    void get_token();
    void serror(int error);
    int is_delim(char c);
public:
    parser();
    double evaluate_expression(char *expression);
};

#endif // PARSER_H
