#include "lexer.hpp"

// Helper function to get token type as string
std::string tokenTypeToString(TokenType type) {
    switch(type) {
        case LEFT_PAREN: return "LEFT_PAREN";
        case RIGHT_PAREN: return "RIGHT_PAREN";
        case LEFT_BRACE: return "LEFT_BRACE";
        case RIGHT_BRACE: return "RIGHT_BRACE";
        case EQUAL_EQUAL: return "EQUAL_EQUAL";
        case NOT_EQUAL: return "NOT_EQUAL";
        case GREATER_EQUALS: return "GREATER_EQUALS";
        case LESSER_EQUALS: return "LESSER_EQUALS";
        case GREATER: return "GREATER";
        case LESSER: return "LESSER";
        case ADD: return "ADD";
        case MUL: return "MUL";
        case SUB: return "SUB";
        case DIV: return "DIV";
        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";
        case DOT: return "DOT";
        case COMMA: return "COMMA";
        case SEMICOLON: return "SEMICOLON";
        case FUN: return "FUN";
        case VAR: return "VAR";
        case EQUALS: return "EQUALS";
        case NUMBER: return "NUMBER";
        case IDENTIFIER: return "IDENTIFIER";
        case STRING: return "STRING";
        case FOR: return "FOR";
        case WHILE: return "WHILE";
        case TRUE: return "TRUE";
        case FALSE: return "FALSE";
        case NIL: return "NIL";
        case EOF_TOKEN: return "EOF";
        default: return "UNKNOWN";
    }
}

int main() {
    std::string source = R"(
        var x = 42;
        var y = 3.14;
        var name = "Breaking Bad";
        if (x > y) { print(name); }
        while (x < 50) { x = x + 1; }
    )";

    lexer lex(source);
    std::vector<Token> tokens = lex.scan_tokens();

    std::cout << "Tokens:\n";
    for(const auto& t : tokens) {
        std::cout << "[" << t.line << "] "
                  << tokenTypeToString(t.type)
                  << " : \"" << t.lexeme << "\"";

        // Print value if it's NUMBER or STRING
        if(t.type == NUMBER) {
            try {
                std::cout << " (value = " << std::any_cast<double>(t.value) << ")";
            } catch(...) {}
        } else if(t.type == STRING) {
            try {
                std::cout << " (value = \"" << std::any_cast<std::string>(t.value) << "\")";
            } catch(...) {}
        }
        std::cout << "\n";
    }

    return 0;
}
