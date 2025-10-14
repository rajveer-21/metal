#ifndef scanner_hpp
#define scanner_hpp
#include <any>
#include <string>
#include <vector>
#include <cctype>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

enum TokenType
{
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, SUB, ADD, SEMICOLON, DIV, MUL,
    NOT,  NOT_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    IDENTIFIER, STRING, NUMBER,
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
    EOF_TOKEN
};

struct Token
{
    std::string lexeme;
    std::any literal;
    TokenType type;
    int line;
    Token(std::string lexeme, std::any literal, TokenType type, int line):
    lexeme(lexeme), literal(literal), type(type), line(line){}
};

class scanner
{
    public:
    scanner(const std::string& source):
    source(source){}
    int start = 0;
    int current = 0;
    int line = 1;
    const std::string& source;
    std::vector<std::shared_ptr<Token>> tokens;
    static std::unordered_map<std::string, TokenType> keywords;
    std::vector<std::shared_ptr<Token>> scan_tokens()
    {
        while(!isAtEnd())
        {
            start = current;
            scan_token();
        }
        tokens.emplace_back(std::make_shared<Token>("", std::any{}, TokenType::EOF_TOKEN, line));
        return tokens;
    }
    bool isAtEnd()
    {
        if(current >= source.size())
        return true;
        return false;
    }
    void add_token(TokenType type, std::any literal = std::any{})
    {
        std::string text = source.substr(start, current - start);
        tokens.emplace_back(std::make_shared<Token>(text, literal, type, line));
    }
    bool match(char expected)
    {
        if(isAtEnd() == true)
        return false;
        if(source[current] != expected)
        return false;
        current++;
        return true;
    }
    char peek()
    {
        if(isAtEnd() == true)
        return '\0';
        else
        return source[current];
    }
    char peek_next()
    {
        if(current + 1 >= source.size())
        return '\0';
        return source[current + 1];
    }
    char advance()
    {
        if(isAtEnd())
        return '\0';
        char curr = source[current];
        current++;
        return curr;
    }
    void scan_token()
    {
        char ch = advance();
        switch(ch)
        {
            case '(': add_token(TokenType::LEFT_PAREN); break;
            case '{': add_token(TokenType::LEFT_BRACE); break;
            case ')': add_token(TokenType::RIGHT_PAREN); break;
            case '}': add_token(TokenType::RIGHT_BRACE); break;
            case ',': add_token(TokenType::COMMA); break;
            case '.': add_token(TokenType::DOT); break;
            case '-': add_token(TokenType::SUB); break;
            case '+': add_token(TokenType::ADD); break;
            case '*': add_token(TokenType::MUL); break;
            case '/': add_token(TokenType::DIV); break;
            case ';': add_token(TokenType::SEMICOLON); break;
            case '!': add_token(match('=') ? TokenType::NOT_EQUAL : TokenType::NOT); break;
            case '=': add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
            case '<': add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
            case '>': add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
            case ' ':
            case '\r':
            case '\t':
            break;
            case '\n':
            line++;
            break;
            case '"': string(); break;
            default:
            {
                if (isdigit(ch)) 
                number();
                else if (isalpha(ch) || ch == '_') 
                identifier();
                else 
                throw std::runtime_error("SYNTAX ERROR : Unexpected character at line " + std::to_string(line));
                break;
            }
        }
    }
    void string()
    {
        while (peek() != '"' && !isAtEnd())
        {
            if (peek() == '\n') line++;
            advance();
        }
        if (isAtEnd()) 
        throw std::runtime_error("SYNTAX ERROR : Unterminated string at line " + std::to_string(line));
        advance();
        std::string value = source.substr(start + 1, current - start - 2);
        add_token(TokenType::STRING, value);
    }
    void number()
    {
        while (isdigit(peek())) 
        advance();
        if (peek() == '.' && isdigit(peek_next()))
        {
            advance();
            while (isdigit(peek())) 
            advance();
        }
        std::string num = source.substr(start, current - start);
        try 
        { 
            add_token(TokenType::NUMBER, std::stod(num)); 
        }
        catch (...) 
        { 
            throw std::runtime_error("RUNTIME ERROR : Unexpected number at line " + std::to_string(line)); 
        }
    }
    void identifier()
    {
        while (isalnum(peek()) || peek() == '_') 
        advance();
        std::string text = source.substr(start, current - start);
        TokenType type = TokenType::IDENTIFIER;
        if (keywords.find(text) != keywords.end()) type = keywords[text];
        add_token(type);
    }
};
std::unordered_map<std::string, TokenType> scanner::keywords = 
{
    {"and", AND}, 
    {"else", ELSE}, 
    {"false", FALSE},
    {"for", FOR}, 
    {"fun", FUN}, 
    {"if", IF}, 
    {"nil", NIL},
    {"or", OR}, 
    {"print", PRINT}, 
    {"return", RETURN},
    {"this", THIS}, 
    {"true", TRUE}, 
    {"var", VAR}, 
    {"while", WHILE}
};
#endif