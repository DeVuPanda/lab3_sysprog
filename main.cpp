#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_set>

enum class TokenType {
    KEYWORD, IDENTIFIER, NUMBER, STRING_LITERAL, CHAR_LITERAL,
    COMMENT, OPERATOR, DELIMITER, FUNCTION, WHITESPACE, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

const std::unordered_set<std::string> operators = {
        "+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">=",
        "+=", "-=", "*=", "/=", "//", "**", "%", "&", "|", "^", "~",
        "and", "or", "not", "is", "in"
};

const std::unordered_set<std::string> delimiters = {
        ",", ":", ";", "(", ")", "[", "]", "{", "}", ".", "\\"
};

bool isKeyword(const std::string& word) {
    static std::vector<std::string> keywords = {
            "def", "return", "if", "else", "elif", "while", "for",
            "import", "from", "as", "class", "try", "except", "with", "in"
    };
    for (const auto& keyword : keywords) {
        if (word == keyword) return true;
    }
    return false;
}

bool isHexDigit(char c) {
    return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool isValidNumberChar(char ch) {
    return isdigit(ch) || ch == '.' || ch == 'e' || ch == 'E' ||
           ch == '+' || ch == '-' || ch == 'x' || ch == 'X' ||
           (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    std::string currentToken;
    size_t i = 0;

    while (i < code.size()) {
        char ch = code[i];

        if (isspace(ch)) {
            tokens.push_back({TokenType::WHITESPACE, std::string(1, ch)});
            ++i;
            continue;
        }

        if (isalpha(ch) || ch == '_') {
            currentToken.clear();
            while (i < code.size() && (isalnum(code[i]) || code[i] == '_')) {
                currentToken += code[i];
                ++i;
            }

            bool isFunction = false;
            size_t j = i;
            while (j < code.size() && isspace(code[j])) j++;
            if (j < code.size() && code[j] == '(') {
                isFunction = true;
                while (j < code.size() && code[j] != ')') j++;
                if (j < code.size()) j++;
            }

            if (isFunction) {
                std::string functionCall = currentToken;
                while (i < j) functionCall += code[i++];
                tokens.push_back({TokenType::FUNCTION, functionCall});
            } else if (isKeyword(currentToken)) {
                tokens.push_back({TokenType::KEYWORD, currentToken});
            } else {
                tokens.push_back({TokenType::IDENTIFIER, currentToken});
            }
            continue;
        }

        if (isdigit(ch) || (ch == '0' && i + 1 < code.size() && (code[i + 1] == 'x' || code[i + 1] == 'X'))) {
            currentToken.clear();
            if (ch == '0' && i + 1 < code.size() && (code[i + 1] == 'x' || code[i + 1] == 'X')) {
                currentToken += code[i++]; // '0'
                currentToken += code[i++]; // 'x' или 'X'
                while (i < code.size() && isHexDigit(code[i])) {
                    currentToken += code[i++];
                }
            } else {
                while (i < code.size() && (isdigit(code[i]) || code[i] == '.')) {
                    currentToken += code[i++];
                }
            }
            tokens.push_back({TokenType::NUMBER, currentToken});
            continue;
        }

        if (ch == '"') {
            currentToken.clear();
            currentToken += ch;
            ++i;
            while (i < code.size() && code[i] != '"') {
                if (code[i] == '\\' && i + 1 < code.size()) {
                    currentToken += code[i++];
                }
                currentToken += code[i++];
            }
            if (i < code.size()) currentToken += code[i++];
            tokens.push_back({TokenType::STRING_LITERAL, currentToken});
            continue;
        }

        if (ch == '\'') {
            currentToken.clear();
            currentToken += ch;
            ++i;
            while (i < code.size() && code[i] != '\'') {
                if (code[i] == '\\' && i + 1 < code.size()) {
                    currentToken += code[i++];
                }
                currentToken += code[i++];
            }
            if (i < code.size()) currentToken += code[i++];
            tokens.push_back({TokenType::CHAR_LITERAL, currentToken});
            continue;
        }

        if (ch == '#') {
            currentToken.clear();
            while (i < code.size() && code[i] != '\n') {
                currentToken += code[i++];
            }
            tokens.push_back({TokenType::COMMENT, currentToken});
            continue;
        }

        if (ispunct(ch)) {
            currentToken.clear();
            currentToken += ch;
            if (i + 1 < code.size()) {
                std::string twoCharOp = currentToken + code[i + 1];
                if (operators.find(twoCharOp) != operators.end()) {
                    currentToken = twoCharOp;
                    i += 2;
                } else {
                    ++i;
                }
            } else {
                ++i;
            }

            if (operators.find(currentToken) != operators.end()) {
                tokens.push_back({TokenType::OPERATOR, currentToken});
            } else if (delimiters.find(currentToken) != delimiters.end()) {
                tokens.push_back({TokenType::DELIMITER, currentToken});
            } else {
                tokens.push_back({TokenType::UNKNOWN, currentToken});
            }
            continue;
        }

        tokens.push_back({TokenType::UNKNOWN, std::string(1, ch)});
        ++i;
    }
    return tokens;
}

void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "Token: " << token.value << " (";
        switch (token.type) {
            case TokenType::KEYWORD: std::cout << "KEYWORD"; break;
            case TokenType::IDENTIFIER: std::cout << "IDENTIFIER"; break;
            case TokenType::NUMBER: std::cout << "NUMBER"; break;
            case TokenType::STRING_LITERAL: std::cout << "STRING_LITERAL"; break;
            case TokenType::CHAR_LITERAL: std::cout << "CHAR_LITERAL"; break;
            case TokenType::COMMENT: std::cout << "COMMENT"; break;
            case TokenType::OPERATOR: std::cout << "OPERATOR"; break;
            case TokenType::DELIMITER: std::cout << "DELIMITER"; break;
            case TokenType::FUNCTION: std::cout << "FUNCTION"; break;
            case TokenType::WHITESPACE: std::cout << "WHITESPACE"; break;
            default: std::cout << "UNKNOWN"; break;
        }
        std::cout << ")\n";
    }
}

int main() {
    std::string code, line;

    std::cout << "Enter Python code (enter 'end' to finish):\n";
    while (std::getline(std::cin, line)) {
        if (line == "end") break;
        code += line + "\n";
    }

    std::vector<Token> tokens = tokenize(code);
    printTokens(tokens);

    return 0;
}