#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_set>
#include <iomanip>

enum class TokenType {
    KEYWORD, IDENTIFIER, NUMBER, STRING_LITERAL, CHAR_LITERAL,
    COMMENT, OPERATOR, DELIMITER, FUNCTION_NAME, WHITESPACE, UNKNOWN
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
            "import", "from", "as", "class", "try", "except", "with", "in", "print", "None"
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

            if (isKeyword(currentToken)) {
                tokens.push_back({TokenType::KEYWORD, currentToken});

                size_t j = i;
                while (j < code.size() && isspace(code[j])) {
                    tokens.push_back({TokenType::WHITESPACE, std::string(1, code[j])});
                    ++j;
                }
                if (j < code.size() && code[j] == '(') {
                    i = j;
                    tokens.push_back({TokenType::DELIMITER, "("});
                    ++i;

                    std::string argsCode;
                    int parenthesesCount = 1;

                    while (i < code.size() && parenthesesCount > 0) {
                        if (code[i] == '(') parenthesesCount++;
                        else if (code[i] == ')') parenthesesCount--;

                        if (parenthesesCount > 0) {
                            argsCode += code[i];
                        }
                        ++i;
                    }

                    if (!argsCode.empty()) {
                        auto argTokens = tokenize(argsCode);
                        tokens.insert(tokens.end(), argTokens.begin(), argTokens.end());
                    }

                    tokens.push_back({TokenType::DELIMITER, ")"});
                }
            } else {
                size_t j = i;
                while (j < code.size() && isspace(code[j])) j++;
                bool isFunction = (j < code.size() && code[j] == '(');

                if (isFunction) {
                    tokens.push_back({TokenType::FUNCTION_NAME, currentToken});

                    while (i < j) {
                        tokens.push_back({TokenType::WHITESPACE, std::string(1, code[i])});
                        ++i;
                    }

                    tokens.push_back({TokenType::DELIMITER, "("});
                    ++i;

                    std::string argsCode;
                    int parenthesesCount = 1;

                    while (i < code.size() && parenthesesCount > 0) {
                        if (code[i] == '(') parenthesesCount++;
                        else if (code[i] == ')') parenthesesCount--;

                        if (parenthesesCount > 0) {
                            argsCode += code[i];
                        }
                        ++i;
                    }

                    if (!argsCode.empty()) {
                        auto argTokens = tokenize(argsCode);
                        tokens.insert(tokens.end(), argTokens.begin(), argTokens.end());
                    }

                    tokens.push_back({TokenType::DELIMITER, ")"});
                } else {
                    tokens.push_back({TokenType::IDENTIFIER, currentToken});
                }
            }
            continue;
        }

        if (isdigit(ch) || (ch == '0' && i + 1 < code.size() && (code[i + 1] == 'x' || code[i + 1] == 'X'))) {
            currentToken.clear();
            if (ch == '0' && i + 1 < code.size() && (code[i + 1] == 'x' || code[i + 1] == 'X')) {
                currentToken += code[i++];
                currentToken += code[i++];
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
    const int columnWidth = 40;

    std::cout << std::string(columnWidth * 2 + 3, '-') << '\n';
    std::cout << '|' << std::setw(columnWidth) << std::left << " <lexeme>"
              << '|' << std::setw(columnWidth) << std::left << " type"
              << "|\n";
    std::cout << std::string(columnWidth * 2 + 3, '-') << '\n';

    for (const auto& token : tokens) {
        std::string lexeme;
        if (token.type == TokenType::WHITESPACE) {
            lexeme = "<[space]>";
        } else {
            lexeme = "<" + token.value + ">";
        }

        std::string tokenType;
        switch (token.type) {
            case TokenType::KEYWORD: tokenType = "KEYWORD"; break;
            case TokenType::IDENTIFIER: tokenType = "IDENTIFIER"; break;
            case TokenType::NUMBER: tokenType = "NUMBER"; break;
            case TokenType::STRING_LITERAL: tokenType = "STRING_LITERAL"; break;
            case TokenType::CHAR_LITERAL: tokenType = "CHAR_LITERAL"; break;
            case TokenType::COMMENT: tokenType = "COMMENT"; break;
            case TokenType::OPERATOR: tokenType = "OPERATOR"; break;
            case TokenType::DELIMITER: tokenType = "DELIMITER"; break;
            case TokenType::FUNCTION_NAME: tokenType = "FUNCTION_NAME"; break;
            case TokenType::WHITESPACE: tokenType = "WHITESPACE"; break;
            default: tokenType = "UNKNOWN"; break;
        }

        std::cout << '|' << " " << std::setw(columnWidth - 1) << std::left << lexeme
                  << '|' << " " << std::setw(columnWidth - 1) << std::left << tokenType
                  << "|\n";
    }

    std::cout << std::string(columnWidth * 2 + 3, '-') << '\n';
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