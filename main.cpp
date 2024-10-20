#include <iostream>
#include <string>
#include <vector>
#include <cctype>

enum class TokenType {
    KEYWORD, IDENTIFIER, NUMBER, SYMBOL, WHITESPACE, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

bool isKeyword(const std::string& word) {
    static std::vector<std::string> keywords = {"if", "else", "while", "for", "def", "return", "import"};
    for (const auto& keyword : keywords) {
        if (word == keyword) return true;
    }
    return false;
}

std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    std::string currentToken;

    for (size_t i = 0; i < code.size(); ++i) {
        char ch = code[i];

        if (isspace(ch)) {
            tokens.push_back({TokenType::WHITESPACE, std::string(1, ch)});
        } else if (isalpha(ch) || ch == '_') {
            currentToken.clear();
            while (isalnum(ch) || ch == '_') {
                currentToken += ch;
                ch = code[++i];
            }
            --i;
            if (isKeyword(currentToken)) {
                tokens.push_back({TokenType::KEYWORD, currentToken});
            } else {
                tokens.push_back({TokenType::IDENTIFIER, currentToken});
            }
        } else if (isdigit(ch)) {
            currentToken.clear();
            while (isdigit(ch)) {
                currentToken += ch;
                ch = code[++i];
            }
            --i;
            tokens.push_back({TokenType::NUMBER, currentToken});
        } else {
            tokens.push_back({TokenType::SYMBOL, std::string(1, ch)});
        }
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
            case TokenType::SYMBOL: std::cout << "SYMBOL"; break;
            case TokenType::WHITESPACE: std::cout << "WHITESPACE"; break;
            default: std::cout << "UNKNOWN"; break;
        }
        std::cout << ")\n";
    }
}

int main() {
    std::string pythonCode, line;

    std::cout << "Enter code on Python (enter 'END' to end the entering code):\n";

    while (std::getline(std::cin, line)) {
        if (line == "END") break;
        pythonCode += line + "\n";
    }

    std::vector<Token> tokens = tokenize(pythonCode);
    printTokens(tokens);

    return 0;
}
