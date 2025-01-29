#include <iostream>
#include <string>
#include <stack>
#include <math.h>
#include <cmath>

using namespace std;

int main(){
    std::string equation;

    //Ask user for an equation to process
    std::cout << "Enter your equation: ";
    std::getline(std::cin, equation);
    
    float result = postfixEval(equation);
}

int isNum(char c){
    if (c >= '0' && c <= '9')
        return 1;
    return -1;
}
int isOperator(char c){
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^')
        return 1;
    return -1;
}
int isBracket(char c){
    if (c == '(')
        return 1;
    if (c == '{')
        return 2;
    if (c == ')')
        return 3;
    if (c == '}')
        return 4;
}
int isTrig(char c1, char c2, char c3){
    if (c1 == 's' && c2 == 'i' && c3 == 'n'){
        return 1;
    } else if (c1 == 'c' && c2 == 'o' && c3 == 's'){
        return 2;
    } else if (c1 == 't' && c2 == 'a' && c3 == 'n'){
        return 3;
    } else if (c1 == 'c' && c2 == 'o' && c3 == 't'){
        return 4;
    }
}
int isLog(char c1, char c2, char c3){
    if (c1 == 'l' && c2 == 'o' && c3 == 'g')
        return 1;
    return -1;
}
int isLn(char c1, char c2){
    if (c1 == 'l' && c2 == 'n')
        return 1;
    return -1;
}
float getNum(char c){
    int val;
    val = c;
    return float(val-'0');
}

float postfixEval(string postfix){
    int a, b;
    stack<float> s;
    string::iterator it;
    for(it=postfix.begin(); it!=postfix.end(); it++){
        if(isOperator(*it) != -1){
            a = s.top();
            s.pop();
            b = s.top();
            s.pop();
            //push result
            //s.push(doOperation(a, b, *it));
        } else if (isNum(*it) != -1){
            s.push(getNum(*it));
        } else if (isTrig(*it, *(it+1), *(it+2)) == 1){
            s.push(sin(*it+4)); // sin(
        } else if (isTrig(*it, *(it+1), *(it+2)) == 2){
            s.push(cos(*it+4));
        } else if (isTrig(*it, *(it+1), *(it+2)) == 3){
            s.push(tan(*it+4));
        } else if (isTrig(*it, *(it+1), *(it+2)) == 4){
            s.push(1/tan(*it+4));
        } else if(isLog(*it, *(it+1), *(it+2)) != -1){
            s.push(log10(*it+4));
        } else if (isLn(*it, *(it+1)) != -1){
            s.push(log(*it+3));
        }
    }
}
