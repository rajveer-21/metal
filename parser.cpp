#ifndef parser_hpp
#define parser_hpp
#include "ExpressionsStatementsEnvironments.hpp"

struct parser
{
struct ParseError : std::runtime_error
{
ParseError(const std::string& message):
std::runtime_error(message){}
};

std::vector<std::shared_ptr<Token>> tokens;
int current = 0;
parser(std::vector<std::shared_ptr<Token>> tokens):
tokens(tokens){} 

std::vector<std::shared_ptr<Stmt>> parse()
{
    std::vector<std::shared_ptr<Stmt>> statements;
    while(!isAtEnd())
    {
        statements.push_back(declaration());
    }
    return statements;
}

std::shared_ptr<Stmt> declaration()
{
    try
    {
        if(match(TokenType::FUN)) return function("function");
        if(match(TokenType::VAR)) return varDeclaration();
        return statement();
    }
    catch(ParseError&)
    {
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> statement()
{
    if(match(TokenType::PRINT)) return printDeclaration();
    if(match(TokenType::WHILE)) return whileDeclaration();
    if(match(TokenType::IF)) return ifDeclaration();
    if(match(TokenType::FOR)) return forDeclaration();
    if(match(TokenType::LEFT_BRACE)) return std::make_shared<BlockStmt>(blockDeclaration());
    return expressionDeclaration();
}

std::vector<std::shared_ptr<Stmt>> blockDeclaration()
{
    std::vector<std::shared_ptr<Stmt>> blockVector;
    while(!isAtEnd() && !check(TokenType::RIGHT_BRACE))
    {
        blockVector.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "COMPILE TIME ERROR : Expected '}' after end of block.");
    return blockVector;
}

std::shared_ptr<Stmt> expressionDeclaration()
{
    auto expr = expression();
    consume(TokenType::SEMICOLON, "COMPILE TIME ERROR : Expected ';' after end of expression.");
    return std::make_shared<ExpressionStmt>(expr);
}

std::shared_ptr<Stmt> ifDeclaration()
{
    consume(TokenType::LEFT_PAREN, "COMPILE TIME ERROR : Expected '(' after 'if' keyword.");
    auto condition = expression(); 
    consume(TokenType::RIGHT_PAREN, "COMPILE TIME ERROR : Expected ')' after end of if statement.");
    auto thenBranch = statement();
    std::shared_ptr<Stmt> elseBranch = nullptr;
    if(match(TokenType::ELSE)) elseBranch = statement();
    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
}

std::shared_ptr<Stmt> printDeclaration()
{
    auto printValue = expression();
    consume(TokenType::SEMICOLON, "COMPILE TIME ERROR : Expected ';' after end of print statement.");
    return std::make_shared<PrintStmt>(printValue);
}

std::shared_ptr<Stmt> varDeclaration()
{
    auto name = consume(TokenType::IDENTIFIER, "COMPILE TIME ERROR : Expected a variable name.");
    std::shared_ptr<Expr> initializer = nullptr;
    if(match(TokenType::EQUALS)) initializer = expression();
    consume(TokenType::SEMICOLON, "COMPILE TIME ERROR : Expected a semicolon after an assignment statement.");
    return std::make_shared<VariableStmt>(name, initializer);
}

std::shared_ptr<Stmt> forDeclaration()
{
    consume(TokenType::LEFT_PAREN, "COMPILE TIME ERROR : Expected '(' after 'for' keyword.");
    std::shared_ptr<Stmt> initializer;
    if(match(TokenType::SEMICOLON)) initializer = nullptr;
    else if(match(TokenType::VAR)) initializer = varDeclaration();
    else initializer = expressionDeclaration();
    std::shared_ptr<Expr> condition;
    if(!check(TokenType::SEMICOLON)) condition = expression();
    consume(TokenType::SEMICOLON, "COMPILE TIME ERROR : Expected ';' after the loop condition.");
    std::shared_ptr<Expr> increment;
    if(!check(TokenType::RIGHT_PAREN)) increment = expression();
    consume(TokenType::RIGHT_PAREN, "COMPILE TIME ERROR : Expected ')' after for clauses.");
    auto body = statement();
    if (increment != nullptr) 
    {
        std::vector<std::shared_ptr<Stmt>> stmts = {body, std::make_shared<ExpressionStmt>(increment)};
        body = std::make_shared<BlockStmt>(stmts);
    }
    if (condition == nullptr) condition = std::make_shared<LiteralExpr>(true);
    body = std::make_shared<WhileStmt>(condition, body);
    if (initializer != nullptr) 
    {
        std::vector<std::shared_ptr<Stmt>> stmts = {initializer, body};
        body = std::make_shared<BlockStmt>(stmts);
    }
    return body;
}

std::shared_ptr<Stmt> whileDeclaration()
{
    consume(TokenType::LEFT_PAREN, "COMPILE TIME ERROR : Expected '(' after a 'while' keyword.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "COMPILE TIME ERROR : Expected ')' after a 'while' condition.");
    auto body = statement();
    return std::make_shared<WhileStmt>(condition, body);
}

std::shared_ptr<Stmt> function(const std::string& kind)
{
    auto name = consume(TokenType::IDENTIFIER, "COMPILE TIME ERROR : Expected a name for " + kind + ".");
    consume(TokenType::LEFT_PAREN, "COMPILE TIME ERROR : Expected '(' after function name.");
    std::vector<std::shared_ptr<Token>> args_list;
    if(!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if(args_list.size() > 255) throw error(peek(), "COMPILE TIME ERROR : Too many arguments.");
            args_list.push_back(consume(TokenType::IDENTIFIER, "COMPILE TIME ERROR : Expected argument name."));
        }
        while(match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "COMPILE TIME ERROR : Expected ')' after function arguments.");
    consume(TokenType::LEFT_BRACE, "COMPILE TIME ERROR : Expected '{' before function body.");
    auto body = blockDeclaration();
    return std::make_shared<FunctionStmt>(name, args_list, body);
}

std::shared_ptr<Expr> expression()
{
    return assignment();
}

std::shared_ptr<Expr> assignment()
{
    auto left_or_expr = orExpr();
    if(match(TokenType::EQUALS))
    {
        auto equals_sign = previous();
        auto right_var_expr = assignment();
        if(auto VarExpr = std::dynamic_pointer_cast<VariableExpr>(left_or_expr))
            return std::make_shared<AssignExpr>(VarExpr->name, right_var_expr);
        throw error(equals_sign, "COMPILE TIME ERROR : Unexpected assignment target.");
    }
    return left_or_expr;
}

std::shared_ptr<Expr> orExpr()
{
    auto left_and_expr = andExpr();
    while(match(TokenType::OR))
    {
        auto or_sign = previous();
        auto right_and_expr = andExpr();
        left_and_expr = std::make_shared<LogicalExpr>(left_and_expr, right_and_expr, or_sign);
    }
    return left_and_expr;
}

std::shared_ptr<Expr> andExpr()
{
    auto left_equality_expr = equality();
    while(match(TokenType::AND))
    {
        auto equality_sign = previous();
        auto right_equality_expr = equality();
        left_equality_expr = std::make_shared<LogicalExpr>(left_equality_expr, right_equality_expr, equality_sign);
    }
    return left_equality_expr;
}

std::shared_ptr<Expr> equality()
{
    auto left_comparison_expr = comparison();
    while(match(TokenType::NOT_EQUAL, TokenType::EQUAL_EQUAL))
    {
        auto equality_operator = previous();
        auto right_comparison_expr = comparison();
        left_comparison_expr = std::make_shared<BinaryExpr>(left_comparison_expr, right_comparison_expr, equality_operator);
    }
    return left_comparison_expr;
}

std::shared_ptr<Expr> comparison()
{
    auto left_term_expr = term();
    while(match(TokenType::GREATER_EQUALS, TokenType::GREATER, TokenType::LESSER, TokenType::LESSER_EQUALS))
    {
        auto comparison_operator = previous();
        auto right_term_expr = term();
        left_term_expr = std::make_shared<BinaryExpr>(left_term_expr, right_term_expr, comparison_operator);
    }
    return left_term_expr;
}

std::shared_ptr<Expr> term()
{
    auto left_factor_expr = factor();
    while(match(TokenType::ADD, TokenType::SUB))
    {
        auto term_operator = previous();
        auto right_factor_expr = factor();
        left_factor_expr = std::make_shared<BinaryExpr>(left_factor_expr, right_factor_expr, term_operator);
    }
    return left_factor_expr;
}

std::shared_ptr<Expr> factor()
{
    auto left_unary_expr = unary();
    while(match(TokenType::DIV, TokenType::MUL))
    {
        auto factor_operator = previous();
        auto right_unary_expr = unary();
        left_unary_expr = std::make_shared<BinaryExpr>(left_unary_expr, right_unary_expr, factor_operator);
    }
    return left_unary_expr;
}

std::shared_ptr<Expr> unary()
{
    if(match(TokenType::SUB, TokenType::NOT))
    {
        auto unary_operator = previous();
        auto right_call_expr = unary();
        return std::make_shared<UnaryExpr>(unary_operator, right_call_expr);
    }
    return call();
}

std::shared_ptr<Expr> call()
{
    auto left_primary_expr = primary();
    while(true)
    {
        if(match(TokenType::LEFT_PAREN))
        {
            left_primary_expr = finishCall(left_primary_expr);
            continue;
        }
        break;
    }
    return left_primary_expr;
}

std::shared_ptr<Expr> finishCall(std::shared_ptr<Expr> callee)
{
    std::vector<std::shared_ptr<Expr>> args;
    if(!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if(args.size() >= 1024) throw error(peek(), "COMPILE TIME ERROR : Too many arguments.");
            args.push_back(expression());
        }
        while(match(TokenType::COMMA));
    }
    auto paren = consume(TokenType::RIGHT_PAREN, "COMPILE TIME ERROR : Expected ')' after arguments.");
    return std::make_shared<CallExpr>(callee, paren, args);
}

std::shared_ptr<Expr> primary()
{
    if(match(TokenType::TRUE)) return std::make_shared<LiteralExpr>(true);
    if(match(TokenType::FALSE)) return std::make_shared<LiteralExpr>(false);
    if(match(TokenType::NIL)) return std::make_shared<LiteralExpr>(nullptr);
    if(match(TokenType::NUMBER, TokenType::STRING)) return std::make_shared<LiteralExpr>(previous()->value);
    if(match(TokenType::LEFT_PAREN))
    {
        auto grouping_expr = expression();
        consume(TokenType::RIGHT_PAREN, "COMPILE TIME ERROR : Expected ')' after grouping expression.");
        return std::make_shared<GroupingExpr>(grouping_expr);
    }
    if(match(TokenType::IDENTIFIER)) return std::make_shared<VariableExpr>(previous());
    throw error(peek(), "COMPILE TIME ERROR : Expected expression.");
}

bool match(TokenType tokenType)
{
    if(check(tokenType))
    {
        advance();
        return true;
    }
    return false;
}
template<typename... Args>
bool match(TokenType tokenType, Args... arguments)
{
    for (TokenType t : {tokenType, arguments...})
    {
        if(check(t))
        {
            advance();
            return true;
        }
    }
    return false;
}
bool check(TokenType tokenType)
{
    if(isAtEnd()) return false;
    return peek()->type == tokenType;
}
std::shared_ptr<Token> consume(TokenType tokenType, const std::string& error_message)
{
    if(check(tokenType)) return advance();
    throw error(peek(), error_message);
}
std::shared_ptr<Token> advance()
{
    if(!isAtEnd()) current++;
    return previous();
}
bool isAtEnd()
{
    return peek()->type == TokenType::EOF_TOKEN;
}
std::shared_ptr<Token> peek()
{
    return tokens[current];
}
std::shared_ptr<Token> previous()
{
    return tokens[current - 1];
}
ParseError error(std::shared_ptr<Token> token, const std::string& error_message)
{
    return ParseError(error_message);
}
void synchronize()
{
    advance();
    while(!isAtEnd())
    {
        if(previous()->type == TokenType::SEMICOLON) return;
        switch(peek()->type)
        {
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::WHILE:
            case TokenType::IF:
            case TokenType::RETURN:
            case TokenType::PRINT:
            return;
            default:
            break;
        }
        advance();
    }
}
