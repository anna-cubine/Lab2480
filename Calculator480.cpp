#include <iostream>
#include <string>

using namespace std;

int main(){
    std::string equation;

    //Ask user for an equation to process
    std::cout << "Enter your equation: ";
    std::getline(std::cin, equation);
    
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
