#ifndef lexer_hpp
#define lexer_hpp
#include <any>
#include <string>
#include <vector>
#include <cctype>
#include <iostream>
#include <unordered_map>

enum TokenType
{
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    EQUAL_EQUAL, NOT_EQUAL, GREATER_EQUALS, LESSER_EQUALS, GREATER, LESSER,
    ADD, MUL, SUB, DIV,
    AND, OR, NOT,
    DOT, COMMA, SEMICOLON,
    FUN, VAR,
    EQUALS,
    NUMBER, IDENTIFIER, STRING,
    FOR, WHILE,
    TRUE, FALSE,
    NIL, EOF_TOKEN,
};
struct Token
{
    std::string lexeme;
    std::any value;
    TokenType type;
    int line;
    Token(const std::string& lexeme, std::any value, TokenType type, int line)
    : lexeme(lexeme), value(value), type(type), line(line){}
};
class lexer
{
    private:
    int lines = 1;
    int start = 0;
    int current = 0;
    std::string source;
    std::vector<Token> tokens;
    static std::unordered_map<std::string, TokenType> keywords;

    public:
    lexer(const std::string& source)
    :source(source){}
    std::vector<Token> scan_tokens()
    {
        while(isAtEnd() == false)
        {
            start = current;
            scan_token();
        }
        tokens.emplace_back("FINISHED", std::any{}, TokenType::EOF_TOKEN, lines);
        return tokens;
    }
    void scan_token()
    {
        char ch = advance();
        switch(ch)
        {
            case '(': add_token(TokenType::LEFT_PAREN); break;
            case ')': add_token(TokenType::RIGHT_PAREN); break;
            case '{': add_token(TokenType::LEFT_BRACE); break;
            case '}': add_token(TokenType::RIGHT_BRACE); break;
            case ';': add_token(TokenType::SEMICOLON); break;
            case ',': add_token(TokenType::COMMA); break;
            case '+': add_token(TokenType::ADD); break;
            case '-': add_token(TokenType::SUB); break;
            case '*': add_token(TokenType::MUL); break;
            case '/': add_token(TokenType::DIV); break;
            case '!':
            {
                if(match('=') == true)
                {
                    add_token(TokenType::NOT_EQUAL);
                    break;
                }
                else
                {
                    add_token(TokenType::NOT);
                    break;
                }
            }
            case '=':
            {
                if(match('=') == true)
                {
                    add_token(TokenType::EQUAL_EQUAL);
                    break;
                }
                else
                {
                    add_token(TokenType::EQUALS);
                    break;
                }
            }
            case '>':
            {
                if(match('=') == true)
                {
                    add_token(TokenType::GREATER_EQUALS);
                    break;
                }
                else
                {
                    add_token(TokenType::GREATER);
                    break;
                }
            }
            case '<':
            {
                if(match('=') == true)
                {
                    add_token(TokenType::LESSER_EQUALS);
                    break;
                }
                else
                {
                    add_token(TokenType::LESSER);
                    break;
                }
            }
            case '\n':
            {
                lines++;
                break;
            }
            case ' ':
            break;
            case '"':
            string();
            break;
            default:
            {
                if(isdigit(ch) == true)
                number();
                else if(isalpha(ch) == true)
                identifier();
                else
                std::cerr << "LEXER_ERROR : Unexpected character at " << lines << std::endl;
            }
        }
    }
    char advance()
    {
        current++;
        return source[current - 1];
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
        return source[current];
    }
    char peek_next()
    {
        if(current + 1 >= source.size())
        return '\0';
        return source[current + 1];
    }
    bool isAtEnd()
    {
        if(current >= source.size())
        return true;
        return false;
    }
    void string()
    {
        while(isAtEnd() == false && peek() != '"')
        {
            if(peek() == '\n')
            lines++;
            advance();
        }
        if(isAtEnd())
        {
        std::cerr << "LEXER ERROR : Unterminated String at line = " << lines << std::endl;
        return;
        }
        advance();
        std::string value = source.substr(start + 1, current - start - 2);
        add_token(TokenType::STRING, value);
    }
    void number()
    {
        while(isdigit(peek()) == true)
        {
            advance();
        }
        if(peek() == '.' && isdigit(peek_next()) == true)
        {
            advance();
            while(isdigit(peek()) == true)
            advance();
        }
        double value = std::stod(source.substr(start, current - start));
        add_token(TokenType::NUMBER, value);
    }
    void identifier()
    {
        while(std::isalpha(peek()) || std::isdigit(peek())) advance();
        std::string value = source.substr(start, current - start);
        TokenType type = TokenType::IDENTIFIER;
        if(keywords.find(value) != keywords.end())
        type = keywords[value];
        add_token(type);
    }
    void add_token(TokenType type, std::any literal = std::any{})
    {
        std::string text = source.substr(start, current - start);
        tokens.emplace_back(text, literal, type, lines);
    }
};
std::unordered_map<std::string, TokenType> lexer::keywords = 
{
    {"while", TokenType::WHILE},
    {"or", TokenType::OR},
    {"and", TokenType::AND},
    {"for", TokenType::FOR},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"nil", TokenType::NIL},
    {"var", TokenType::VAR},
    {"fun", TokenType::FUN}
};
#endif