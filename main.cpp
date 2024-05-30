#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <unordered_map>

using namespace std;


int precedence(char op) {
    if(op == '+' || op == '-') {
        return 1;
    }
    if(op == '*' || op == '/') {
        return 2;
    }
    if(op == '^') {
        return 3;
    }
    return 0;
}

// Odiy ifodalar uchun
double applyOp(double a, double b, char op) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if(b == 0) throw std::runtime_error("Kasr son maxraji 0 bo'lishi mumkin emas");
            return a / b;
        case '^': return std::pow(a, b);
    }
    return 0;
}

// Funksiyalar uchun
double applyFunction(const std::string& func, double value) {
    if(func == "sin") return std::sin(value);
    if(func == "cos") return std::cos(value);
    if(func == "log") return std::log(value);
    if(func == "tg") return std::tan(value);
    if(func == "ln") return std::log(value);
    if(func == "sqrt") return std::sqrt(value);
    throw std::runtime_error("Funksiya topilmadi: " + func);
}


// Odiy Qiymatlar uchun
double evaluate(const std::string& tokens) {
    int i;
    // stack ochib hisoblash
    std::stack<double> values;
    std::stack<char> ops;
    
    for(i = 0; i < tokens.length(); i++) {
        
        if(tokens[i] == ' ') {
            continue;
        }
        
        else if(isdigit(tokens[i])) {
            double val = 0;
            
            while(i < tokens.length() && isdigit(tokens[i])) {
                val = (val * 10) + (tokens[i] - '0');
                i++;
            }
            
            if(i < tokens.length() && tokens[i] == '.') {
                i++;
                double frac = 0.1;
                while(i < tokens.length() && isdigit(tokens[i])) {
                    val += (tokens[i] - '0') * frac;
                    frac /= 10;
                    i++;
                }
            }
            
            values.push(val);
            i--;
        }
        
        else if(tokens[i] == '(') {
            ops.push(tokens[i]);
        }
        
        else if(tokens[i] == ')') {
            while(!ops.empty() && ops.top() != '(') {
                double val2 = values.top();
                values.pop();
                
                double val1 = values.top();
                values.pop();
                
                char op = ops.top();
                ops.pop();
                
                values.push(applyOp(val1, val2, op));
            }
            
            if(!ops.empty()) {
                ops.pop();
            }
        }
        
        else {
            while(!ops.empty() && precedence(ops.top()) >= precedence(tokens[i])) {
                double val2 = values.top();
                values.pop();
                
                double val1 = values.top();
                values.pop();
                
                char op = ops.top();
                ops.pop();
                
                values.push(applyOp(val1, val2, op));
            }
            
            ops.push(tokens[i]);
        }
    }
    
    while(!ops.empty()) {
        double val2 = values.top();
        values.pop();
        
        double val1 = values.top();
        values.pop();
        
        char op = ops.top();
        ops.pop();
        
        values.push(applyOp(val1, val2, op));
    }
    
    return values.top();
}

// Murakab funksiyanallar uchun
double evaluate(const std::string& tokens, double x) {
    int i;
    std::stack<double> values;
    std::stack<char> ops;
    std::stack<std::string> functions;

    for(i = 0; i < tokens.length(); i++) {
        if(tokens[i] == ' ') continue;

        if(tokens[i] == 'x') {
            values.push(x);
        } 
        else if(isdigit(tokens[i]) || tokens[i] == '.') {
            double val = 0;
            std::stringstream ss;

            while(i < tokens.length() && (isdigit(tokens[i]) || tokens[i] == '.')) {
                ss << tokens[i];
                i++;
            }
            i--;

            ss >> val;
            values.push(val);
        } 
        else if(tokens[i] == '(') {
            ops.push(tokens[i]);
        } 
        else if(tokens[i] == ')') {
            while(!ops.empty() && ops.top() != '(') {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();

                values.push(applyOp(val1, val2, op));
            }

            if(!ops.empty()) ops.pop();

            if(!functions.empty()) {
                double val = values.top(); values.pop();
                std::string func = functions.top(); functions.pop();

                values.push(applyFunction(func, val));
            }
        } 
        else if(isalpha(tokens[i])) {
            std::string func;
            while(i < tokens.length() && isalpha(tokens[i])) {
                func.push_back(tokens[i]);
                i++;
            }
            i--;

            functions.push(func);
        } 
        else {
            while(!ops.empty() && precedence(ops.top()) >= precedence(tokens[i])) {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();

                values.push(applyOp(val1, val2, op));
            }

            ops.push(tokens[i]);
        }
    }

    while(!ops.empty()) {
        double val2 = values.top(); values.pop();
        double val1 = values.top(); values.pop();
        char op = ops.top(); ops.pop();

        values.push(applyOp(val1, val2, op));
    }

    return values.top();
}

int main() {
    std::string expression = "sqrt(1+pow(cos(x),2))";
    double x = 2.0;

    try {
        double result = evaluate(expression, x);
        std::cout << "Funksiyanal ko'rinishi: " << expression << ", Result: " << result << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "OOPS )), Xatolik: " << e.what() << std::endl;
    }
    return 0;
}