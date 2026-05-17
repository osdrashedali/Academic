%{
#include <stdio.h>
#include <stdlib.h>

#define MAX 100

double valueStack[MAX];
char operatorStack[MAX];

int valueTop = -1;
int operatorTop = -1;

/* Push value into stack */
void pushValue(double val) {
    valueStack[++valueTop] = val;
}

/* Push operator into stack */
void pushOperator(char op) {
    operatorStack[++operatorTop] = op;
}

/* Pop value */
double popValue() {
    return valueStack[valueTop--];
}

/* Pop operator */
char popOperator() {
    return operatorStack[operatorTop--];
}

/* Check operator priority */
int precedence(char op) {

    if(op == '+' || op == '-')
        return 1;

    if(op == '*' || op == '/')
        return 2;

    return 0;
}

/* Perform calculation */
void applyOperator() {

    double b = popValue();
    double a = popValue();

    char op = popOperator();

    if(op == '+')
        pushValue(a + b);

    else if(op == '-')
        pushValue(a - b);

    else if(op == '*')
        pushValue(a * b);

    else if(op == '/')
        pushValue(a / b);
}
%}

%%

/* Number */
[0-9]+(\.[0-9]+)? {
    pushValue(atof(yytext));
}

/* Operator */
[+\-*/] {

    while(operatorTop >= 0 &&
          precedence(operatorStack[operatorTop]) >= precedence(yytext[0])) {

        applyOperator();
    }

    pushOperator(yytext[0]);
}

/* Left bracket */
"(" {
    pushOperator('(');
}

/* Right bracket */
")" {

    while(operatorTop >= 0 &&
          operatorStack[operatorTop] != '(') {

        applyOperator();
    }

    popOperator();
}

/* Ignore spaces */
[ \t\r]+ ;

/* Enter key */
\n {

    while(operatorTop >= 0)
        applyOperator();

    printf("Result = %lf\n", popValue());

    valueTop = -1;
    operatorTop = -1;
}

/* Ignore unknown characters */
. ;

%%

int main() {

    printf("Desk Calculator\n");
    printf("Enter Expression:\n");

    yylex();

    return 0;
}
