#include <cstring>
#include <stdlib.h>
#include <cctype>
#include "parser.h"
#include "QString"
#include "QStringList"
#include "QRegExp"

/* parser constructor */

parser::parser()
{
    expression_ptr = NULL;
}


double parser::evaluate_expression(char *expression)
{
    double result;

    // convert hex
    QString s(expression);
    bool ok;
//    s.replace(QRegExp("(0x[0-9A-F]+)"),QString("%1").arg(QString("\\1").toUInt(&ok,16)));

    QRegExp rx;
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    int pos =0;

    rx.setPattern("(0x[0-9A-F]+)"); //
    while ((pos = rx.indexIn(s, pos)) != -1) {
        pos = rx.indexIn(s,pos);
        QString f = rx.cap(0);
        s.replace(f,QString("%1").arg(f.toUInt(&ok,16)));
     }

    rx.setPattern("(0b[01]+)"); //
    pos=0;
    while ((pos = rx.indexIn(s, pos)) != -1) {
        pos = rx.indexIn(s,pos);
        QString f = rx.cap(0);
        s.replace(f,QString("%1").arg(f.mid(2).toUInt(&ok,2)));
     }

    QByteArray   bytes  = s.toAscii();
    char * ptr    = bytes.data();
    expression_ptr = ptr;
    get_token();
    if(!*token) {
        serror(2);
        return 0.0;
    }
    evaluate_expression2(result);
    if(*token) serror(0);
    return result;
}

void parser::evaluate_expression2(double &result)
{
    register char op;
    double temp;
    evaluate_expression3(result);
    while((op = *token) == '+' || op == '-')
    {
        get_token();
        evaluate_expression3(temp);
        switch(op)
        {
        case '-':
            result = result - temp;
            break;
        case '+':
            result = result + temp;
            break;
        }
    }
}

void parser::evaluate_expression3(double &result)
{
    register char op;
    double temp;
    evaluate_expression4(result);
    while((op = *token) == '*' || op == '/' || op == '%')
    {
        get_token();
        evaluate_expression4(temp);
        switch(op)
        {
        case '*':
            result = result * temp;
            break;
        case '/':
            result = result / temp;
            break;
        case '%':
            result = (int) result % (int) temp;
            break;
        }
    }
}

void parser::evaluate_expression4(double &result)
{
    double temp, ex;
    register int t;
    evaluate_expression5(result);
    if(*token== '^')
    {
        get_token();
        evaluate_expression4(temp);
        ex = result;
        if(temp==0.0)
        {
            result = 1.0;
            return;
        }
        for(t=(int)temp-1; t>0; --t) result = result * (double)ex;
    }
}

void parser::evaluate_expression5(double &result)
{
    register char op;
    op = 0;
    if((token_type == DELIMITER) && *token=='+' || *token == '-')
    {
        op = *token;
        get_token();
    }
    evaluate_expression6(result);
    if(op=='-') result = -result;
}

void parser::evaluate_expression6(double &result)
{
    if((*token == '('))
    {
        get_token();
        evaluate_expression2(result);
        if(*token != ')')
            serror(1);
        get_token();
    }
    else atom(result);
}


void parser::atom(double &result)
{
    switch(token_type)
    {
    case NUMBER:
        result = atof(token);
        get_token();
        return;
    default:
        serror(0);
    }
}

/* Syntax error Display */

void parser::serror(int error)
{
    static char *e[]= {
        "Syntax Error",
        "Unbalanced Parentheses",
        "No expression Present"
    };
    //cout << e[error] << endl;
}

void parser::get_token()
{
    register char *temp;
    token_type = 0;
    temp = token;
    *temp = '\0';
    if(!*expression_ptr) return; // at end of expression
    while(isspace(*expression_ptr)) ++expression_ptr; // skip over white space
    if(strchr("+-*/%^=()", *expression_ptr))
    {
        token_type = DELIMITER;
        // advance to next char
        *temp++ = *expression_ptr++;
    }
    else if(isalpha(*expression_ptr))
    {
        while(!is_delim(*expression_ptr)) *temp++ = *expression_ptr++;
        token_type = VARIABLE;
    }
    else if(isdigit(*expression_ptr))
    {
        while(!is_delim(*expression_ptr)) *temp++ = *expression_ptr++;
        token_type = NUMBER;
    }
    *temp = '\0';
}

int parser::is_delim(char c)
{
    if(strchr(" +-/*%^=()", c) || c==9 || c=='\r' || c==0)
        return 1;
    return 0;
}


//int main()
//{
//    char exp_str[50];
//    cout << "Enter a value to stop"<parser ob;
//    for(;;)
//    {
//        cout << "Enter the Expression "<	cin.getline(exp_str, 49);
//        if(*exp_str=='.')
//            break;
//        cout << "Result is : " << ob.evaluate_expression(exp_str) << endl<};
//    return 0;

