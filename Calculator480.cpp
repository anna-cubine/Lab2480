#include <iostream>
#include <string>
#include <stack>
#include <math.h>
#include <cmath>
#include <vector>
#include <cctype>
#include <unordered_set>
#include <unordered_map>

using namespace std;

/**
 * Enum for the type of token to simplify tokenization
 */
enum TokenType {
    NUM, OPERATOR, FUNCTION, PARENTHESIS
};
/**
 * Token struct so each token has a given type and a value along with it
 */
struct Token {
    TokenType type;
    string value;
};

/**
 * Unordered map containing an operator and the precedence of each
 */
std::unordered_map<std::string, int> precedence = {
    {"+", 1}, {"-", 1},
    {"*", 2}, {"/", 2},
    {"^", 3}
};

std::unordered_map<std::string, bool> isLeftAssociative = {
    {"+", true}, {"-", true},
    {"*", true}, {"/", true},
    {"^", false}
};

int main(){
    std::string expression;
    bool active = true;
    while (active){
        //Ask user for an equation to process
        std::cout << "Enter your expression or type exit to end program: ";
        std::getline(std::cin, expression);

        //Since I can't change the entire string to lowercase at once in c++(at least to my knowledge), I first
        //check if the user has e or E at the beginning of their expression, then I put everything to lowercase
        //and check from there if the expression is "exit". If so, end the program.
        if (expression[0] == 'e' || expression[0] == 'E'){
            for (int i=0; i<expression.length(); i++)
                tolower(expression[i]);
            if (expression == "exit"){
                std::cout << "Exiting...";
                active = false;
            }
        }
        //If the user has not typed exit, then we will go on with tokenizing the expression.
        Tokenizer Tokenizer(expression);
    }
    return 0;
}

/**
 * Here is an entire class dedicated to tokenizing the expression given by the user. I had a lot of help from ChatGPT since I don't remember how to
 * tokenize in Java and I'm still very new to C++. I made sure to have an understanding of the code before using it however. 
 */
class Tokenizer {
public:
    /**
     * Explicit constructor for the Tokenizer class, keeps from accidental conversions. Also initializing variables for the while loop
     * @param expression given by user
     */
    explicit Tokenizer (const std::string& expression) : expr(expression), pos(0){}

    /**
     * Method that takes the initializer list variables that have the expression eithin them, and goes through a loop, tokenizing and parsing
     * everything within it, taking unexpected characters into account and throwing a runtime error with the location of the error when caught. 
     * @return vector of parsed tokens, ready to be evaluated!
     */
    std::vector<Token> tokenize() {
        vector<Token> tokens;
        while(pos < expr.length()){
            //If the char is just a space character, the skip the space, will only allow one space in between chars.
            //If user puts multiple spaces at once, there will be an error thrown in the else clause inside the while loop.
            if (std::isspace(expr[pos])){
                pos++;
                continue;
            }
            //If char at position is a digit OR if it is a decimal, then also must not be the end of the expression AND the char
            //afterwards must be a digit
            if(std::isdigit(expr[pos]) || (expr[pos] == '.' && pos + 1 < expr.length() && std::isdigit(expr[pos + 1]))){
                //push_back inserts item into the token vector
                tokens.push_back(parseNumber());
            } else if (std::isalpha(expr[pos])){
                tokens.push_back(parseFunctionOrVariable());
            } else if (isOperator(expr[pos])){
                tokens.push_back(parseOperator(tokens));
            } else if (isBracket(expr[pos])){
                tokens.push_back({TokenType::PARENTHESIS, std::string(1, expr[pos++])});
            } else {
                if (std::isspace(expr[pos])){
                    throw std::runtime_error("Please only use one space in between characters: " + std::string(1, expr[pos]));
                }
                throw std::runtime_error("Unexpected character in expression: " + std::string(1, expr[pos]));
            }
        }
    }

/**
 * This private secion of the class is created to hold methods for checking tokens and parsing them
 */
private:
    std::string expr;
    //Using size_t because that is usually the type of length and size in c++. Didn't want any
    //unnecessary bugs
    size_t pos;

    //Created a set to hold all my types of 'word functions'
    std::unordered_set<std::string> functions {"sin", "cos", "tan", "cot", "ln", "log"};

    /**
     * Method that parses the number and decimal char. Moves up the position in the while loop in the tokenize method, grabbing any
     * consecutive numbers/decimals.
     * @return token type of number and a substring of whatever was just parsed.
     */
    Token parseNumber() {
        size_t start = pos;
        while (pos < expr.length() && (std::isdigit(expr[pos]) || expr[pos] == '.')){
            pos++;
        }
        return {TokenType::NUM, expr.substr(start, pos - start)};
    }
    /**
     * Method that parses the word functions. Moves up the position in the while loop in the tokenize method, grabbing all the chars
     * from the alphabetical string. Then it will turn it into a string to make sure it is one of the actual function and not just
     * a random set of chars. If it's not one of the functions in the string, it will throw an error, otherwise...
     * @return token type function and the substring name of the function
     */
    Token parseFunctionOrVariable() {
        size_t start = pos;
        while (pos < expr.length() && std::isalpha(expr[pos])){
            pos++;
        }
        std::string name = expr.substr(start, pos - start);
        if (functions.count(name)){
            return {TokenType::FUNCTION, name};
        }
        throw std::runtime_error("Unknown Function: " + name);
    }
    /**
     * Method that parses the operator. Here, we need to look at the context around it to fully understand the operator.
     * If there have yet to be any tokens and the operator is a -, OR the previous token was another operator OR a parenthesis, then we
     * consider this ALONG with this whole time the operator being a -, then we consider this a negative operator, and it will be parsed
     * in a different method, otherwise
     * @return token type operator along with the string representation of the operator
     */
    Token parseOperator(const std::vector<Token>& tokens) {
        char op = expr[pos];
        if (op == '-' && (tokens.empty() || tokens.back().type == TokenType::OPERATOR || (tokens.back().value == "(" || tokens.back().value == "{"))){
            return parseNegativeNumber();
        }
        pos++;
        return {TokenType::OPERATOR, std::string(1, op)};
    }
    /**
     * Method that accounts for negative numbers. First, we skip the -, then we parse the number in the next position, after that we simply
     * add a - to the value of the number
     * @return negative version of next number in expression
     */
    Token parseNegativeNumber() {
        pos++; //Don't parse '-'
        Token number = parseNumber();
        number.value = "-" + number.value;
        return number;
    }
    /**
     * Method that simply returns a true/false of if the char given matches any of the shown operators
     * @return true if given char matches any of these operators, false if not
     */
    bool isOperator(char c){
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
    }
    /**
     * Method that simply returns a true/false of if the char given matches any of the shown brackets/parenthesis
     * @return true if given char matches any of these brackets, false if not
     */
    bool isBracket(char c){
        return c == '(' || c == ')' || c == '{' || c == '}';
    }
};

/**
 * This method takes the tokenized equation and changes it from infix to postfix while also accounting for precedence of
 * operations. With this, we can evaluate the equation much easier. Again, I got help from chatGPT and got some help from this
 * github page: https://gist.github.com/t-mat/b9f681b7591cdae712f6 using chatGPT to help personalize it to my tokenizer I created
 * before.
 * @return postfix version of tokenized equation
 */
std::vector<Token> evaluateTokens(const std::vector<Token>& tokens){
    std::vector<Token> outputQueue;
    std::stack<Token> operatorStack;
    
    //Going through each token in equation
    for(const auto& token : tokens){
        if (token.type == TokenType::NUM){
            outputQueue.push_back(token); //Numbers go to output
        } else if (token.type == TokenType::FUNCTION){
            operatorStack.push(token);
        } else if (token.type == TokenType::OPERATOR){
            while (!operatorStack.empty() && operatorStack.top().type == TokenType::OPERATOR){
                std::string topOp = operatorStack.top().value;
                if ((isLeftAssociative[token.value] && precedence[token.value] <= precedence[topOp]) ||
                    (!isLeftAssociative[token.value] && precedence[token.value] < precedence[topOp])) {
                    outputQueue.push_back(operatorStack.top());
                    operatorStack.pop();
                } else {
                    break;
                }
            }
            operatorStack.push(token);
        } else if (token.value == "(" || token.value == "{"){
            operatorStack.push(token); //Open parenthesis in stack
        } else if (token.value == ")" || token.value == "}"){
            while (!operatorStack.empty() && operatorStack.top().value != "(" || operatorStack.top().value != "{"){
                outputQueue.push_back(operatorStack.top());
                operatorStack.pop();
            }
            if (operatorStack.empty()){
                throw std::runtime_error("Mismatched parentheses!");
            }
            operatorStack.pop();
            if (!operatorStack.empty() && operatorStack.top().type == TokenType:: FUNCTION){
                outputQueue.push_back(operatorStack.top());
                operatorStack.pop();
            }
        }
    }

    while (!operatorStack.empty()){
        if (operatorStack.top().value == "("){
            throw std::runtime_error("Mismatched Parentheses!");
        }
        outputQueue.push_back(operatorStack.top());
        operatorStack.pop();
    }

    return outputQueue;
}
