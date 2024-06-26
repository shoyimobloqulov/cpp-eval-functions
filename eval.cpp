#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <vector>

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

double applyPow(double base, double exponent) {
    return std::pow(base, exponent);
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

            if(func == "pow") {
                if (tokens[i + 1] != '(') throw std::runtime_error("POW dan foydalanishdagi xatolik.");
                int j = i + 2; 
                std::stack<char> parentheses;
                parentheses.push('(');
                std::string baseExpr, exponentExpr;
                bool commaFound = false;

                while (!parentheses.empty() && j < tokens.length()) {
                    if (tokens[j] == '(') {
                        parentheses.push('(');
                    } else if (tokens[j] == ')') {
                        parentheses.pop();
                    } else if (tokens[j] == ',' && parentheses.size() == 1) {
                        commaFound = true;
                        j++;
                        continue;
                    }
                    
                    if (!commaFound) {
                        baseExpr += tokens[j];
                    } else {
                        exponentExpr += tokens[j];
                    }

                    j++;
                }

                if (!parentheses.empty()) throw std::runtime_error("Mismatched parentheses in pow function");

                double base = evaluate(baseExpr, x);
                double exponent = evaluate(exponentExpr, x);
                values.push(applyPow(base, exponent));
                i = j - 1;
            } else {
                functions.push(func);
            }
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
    string s;

    cin >> s;

    double a,b,n;
    cin >> a >> b >> n;

    double mod = abs(b - a) / n;

    // [a,b]
    vector<double>array;

    try {
        for (double i = a; i <= b; i+=mod)
        {
            double x = evaluate(s,i);
            array.push_back(x);
        }

        cout << "Qiymatlar: ";
        for (int i = array.size() - 1; i >= 0; i--)
        {
            cout << array[i] << " ";
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "OOPS )), Xatolik: " << e.what() << std::endl;
    }
    return 0;
}