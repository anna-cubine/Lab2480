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

//To run :  g++ Calculator480.cpp -o Calculator.exe

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
            while (std::isspace(expr[pos])){
                pos++;
            }
            //If char at position is a digit OR if it is a decimal, then also must not be the end of the expression AND the char
            //afterwards must be a digit
            if(std::isdigit(expr[pos]) || (expr[pos] == '.' && pos + 1 < expr.length() && std::isdigit(expr[pos + 1]))){
                //push_back inserts item into the token vector
                tokens.push_back(parseNumber());
            } else if (std::isalpha(expr[pos])){
                tokens.push_back(parseFunctionOrVariable());
            } else if (isOperator(expr[pos])){
                //This is to check if we're evaluating a negative rather than subtraction
                if (expr[pos] == '-' && (pos == 0 || (tokens.back().type == TokenType::OPERATOR || (tokens.back().value == "(" || tokens.back().value == "{")))){
                    size_t start = pos++;
                    //Here, we're taking the number along with any possible decimals and putting it all together
                    //in tokenized form.
                    while (pos < expr.length() && (isdigit(expr[pos]) || expr[pos] == '.')){
                        pos++;
                    }
                    tokens.push_back({TokenType::NUM, expr.substr(start, pos - start)});
                }
                //Have to account for space when taking the next operator
                while (std::isspace(expr[pos])){
                    pos++;
                }
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
        return tokens;
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

    //Going through each token in equation. Must use constant so it remains the same
    //and auto& so there are no copies which wouldn't be very efficient.
    for(const auto& token : tokens){
        if (token.type == TokenType::NUM){
            outputQueue.push_back(token); //Numbers go to output queue
        } else if (token.type == TokenType::FUNCTION){
            operatorStack.push(token); //functions go to operator stack
        } else if (token.type == TokenType::OPERATOR){
            //If the operatorStack is not empty and the top has an aoperator
            while (!operatorStack.empty() && operatorStack.top().type == TokenType::OPERATOR){
                //First, put the operator into a new variable
                std::string topOp = operatorStack.top().value;
                //if the operator is evaluated left to right and the precedence is less or equal to the last
                //operator evaluated OR it's evaluated right to left and the precedence is less than the last operator
                if ((isLeftAssociative[token.value] && precedence[token.value] <= precedence[topOp]) ||
                    (!isLeftAssociative[token.value] && precedence[token.value] < precedence[topOp])) {
                    //Then we push the top of the operator stack onto the output queue and pop it from the op stack
                    outputQueue.push_back(operatorStack.top());
                    operatorStack.pop();
                } else {
                    //If it's neither, then just skip
                    break;
                }
            }
            //Push to stack
            //We're not pushing this to the operator queue yet because we need to evaluate precedences before
            //pushing them tho the output queue. We go from most precedence to least
            operatorStack.push(token);
        } else if (token.value == "(" || token.value == "{"){
            operatorStack.push(token); //Open parenthesis in stack
        } else if (token.value == ")" || token.value == "}"){
            //Making sure that there is an open parentheses per close parentheses
            while (!operatorStack.empty() && (operatorStack.top().value != "(" && operatorStack.top().value != "{")){
                //Push open parenthesis to output queue and pop it from the stack
                outputQueue.push_back(operatorStack.top());
                operatorStack.pop();
            }
            //If there are no open parentheses, throw an error
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

    //In case there are some missing operators or parentheses
    while (!operatorStack.empty()){
        if (operatorStack.top().value == "(" || operatorStack.top().value == "{"){
            throw std::runtime_error("Mismatched Parentheses!");
        }
        outputQueue.push_back(operatorStack.top());
        operatorStack.pop();
    }

    return outputQueue;
}

/**
 * Postfix evaluation which I got help form chatGPT. This will completely evaluate the tokenized and postfixed expression
 * and return a result. We will iterate through the tokens and evaluate per function or operator given
 * @return result from expression given by user
 */
float postfixEval(const std::vector<Token>& tokens){
    std::stack<float> evalStack;

    for(const auto& token : tokens){
        if (token.type == TokenType::NUM){
            evalStack.push(std::stoi(token.value)); //Push number to an evaluation stack to work with the operators and functions
        } else if (token.type == TokenType::OPERATOR){
            //If there is simply just a number or something, alert the user
            if (evalStack.size() < 2){
                throw std::runtime_error("Not enough operands!");
            }

            //Push the eval numbers to a and b, making sure the first goes to b and second to a
            float b = evalStack.top(); evalStack.pop();
            float a = evalStack.top(); evalStack.pop();
            float result = 0;

            if (token.value == "+") result = a + b;
            else if (token.value == "-") result = a - b;
            else if (token.value == "*") result = a * b;
            else if (token.value == "/") {
                //Accounting for division by 0
                if (b == 0) throw std::runtime_error("Division by 0");
                result = a / b;
            }
            else if (token.value == "^") result = std::pow(a, b);

            evalStack.push(result); //Result pushed to evaluation stack so it can be evaluated later
        } else if (token.type == TokenType::FUNCTION){
            //If there's a function but there's no number for it, throw error
            if (evalStack.empty()) {
                throw std::runtime_error("Function needs an argument!");
            }

            float arg = evalStack.top(); evalStack.pop();
            float result = 0;

            if (token.value == "sin") result = std::sin(arg);
            else if (token.value == "cos") result = std::cos(arg);
            else if (token.value == "tan") result = std::tan(arg);
            else if (token.value == "cot") result = 1 / std::tan(arg);
            else if (token.value == "ln") result = std::log(arg);
            else if (token.value == "log") result = std::log10(arg);

            evalStack.push(result);
        }
    }

    if (evalStack.size() != 1)
        throw std::runtime_error("Invalid expression!");
    
    //Return evaluated soluation!
    return evalStack.top();
}

int main(){
    std::string expression;
    
    while (true){
        try {
            //Ask user for an equation to process
            std::cout << "Enter your expression or type exit to end program: ";
            std::getline(std::cin, expression);

            //Since I can't change the entire string to lowercase at once in c++(at least to my knowledge), I first
            //check if the user has e or E at the beginning of their expression, then I put everything to lowercase
            //and check from there if the expression is "exit". If so, end the program.
            if (!expression.empty() && expression[0] == 'e' || expression[0] == 'E'){
                for (auto& i : expression)
                    i = tolower(i);
                if (expression == "exit"){
                    std::cout << "Exiting...\n";
                    break;
                }
            }
            //If the user has not typed exit, then we will go on with tokenizing the expression.
            Tokenizer tokenizer(expression);
            std::vector<Token> tokens = tokenizer.tokenize();

            //Converting to reverse polish notation suing Shunting Yard algorithm
            std::vector<Token> postfix = evaluateTokens(tokens);

            //Evaluate the expression
            float result = postfixEval(postfix);
            
            //Display result
            std::cout << result << std::endl;
        } catch (const std::runtime_error& e){
            std::cerr << "Error: " << e.what() << "\nPlease enter a valid expression.\n";
            continue;
        }
    }
    return 0;
}