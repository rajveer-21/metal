#ifndef parser_hpp
#define parser_hpp
#include "scanner.hpp"
struct Expr;
struct Binary;
struct Assign;
struct Unary;
struct Grouping;
struct Literal;
struct Variable;
struct ExprVisitor;
struct Stmt;
struct Print;
struct Var;
struct Expression;
struct StmtVisitor;
struct Environment;
class RuntimeError; 
void metal_error(std::shared_ptr<Token> token, const std::string& message);
void metal_runtime_error(const RuntimeError& error);
class ParseError : public std::runtime_error
{
    public:
    ParseError():
    std::runtime_error(""){}
};
class RuntimeError : public std::runtime_error 
{
    public:
    std::shared_ptr<Token> token;
    RuntimeError(std::shared_ptr<Token> token, const std::string& message):
    std::runtime_error(message), token(token){}
};
struct Environment
{
    std::unordered_map<std::string, std::any> values;
    void define(std::string name, std::any value)
    {
        values[name] = value;
    }
    std::any get(const std::shared_ptr<Token>& token)
    {
        auto value = values.find(token->lexeme);
        if(value != values.end())
        return value->second;
        else
        throw RuntimeError(token, "Undefined Variable");
    }
    void assign(const std::shared_ptr<Token>& token, std::any value)
    {
        if (values.count(token->lexeme)) 
        {
            values[token->lexeme] = value;
            return;
        }
        throw RuntimeError(token, 
        "Undefined variable '" + token->lexeme + "'.");
    }
};
struct Expr
{
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) = 0;
};
struct ExprVisitor 
{
    virtual ~ExprVisitor() = default;
    virtual std::any visitUnaryExpr(const Unary& expr) = 0;
    virtual std::any visitBinaryExpr(const Binary& expr) = 0;
    virtual std::any visitAssignExpr(const Assign& expr) = 0;
    virtual std::any visitLiteralExpr(const Literal& expr) = 0;
    virtual std::any visitGroupingExpr(const Grouping& expr) = 0;
    virtual std::any visitVariableExpr(const Variable& expr) = 0;
};
struct Binary : Expr
{
    std::shared_ptr<Expr> left;
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;
    Binary(std::shared_ptr<Expr> left, std::shared_ptr<Token> op, std::shared_ptr<Expr> right):
    left(left), op(op), right(right){}
    std::any accept(ExprVisitor& visitor)
    {
        return visitor.visitBinaryExpr(*this);
    }
};
struct Unary : Expr 
{
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;
    Unary(std::shared_ptr<Token> op, std::shared_ptr<Expr> right):
    op(op), right(right){};
    std::any accept(ExprVisitor& visitor)
    {
        return visitor.visitUnaryExpr(*this);
    }
};
struct Grouping : Expr 
{
    std::shared_ptr<Expr> expression;
    Grouping(std::shared_ptr<Expr> expression):
    expression(expression){}
    std::any accept(ExprVisitor& visitor)
    {
        return visitor.visitGroupingExpr(*this);
    }
};
struct Literal : Expr 
{
    std::any value;
    Literal(std::any value):
    value(value){}
    std::any accept(ExprVisitor& visitor)
    {
        return visitor.visitLiteralExpr(*this);
    }
};
struct Variable : Expr 
{
    std::shared_ptr<Token> token;
    Variable(std::shared_ptr<Token> token):
    token(token){}
    std::any accept(ExprVisitor& visitor)
    {
        return visitor.visitVariableExpr(*this);
    }
};
struct Assign : Expr 
{
    std::shared_ptr<Token> token;
    std::shared_ptr<Expr> expression;
    Assign(std::shared_ptr<Token> token, std::shared_ptr<Expr> expression):
    token(token), expression(expression){}
    std::any accept(ExprVisitor& visitor)
    {
        return visitor.visitAssignExpr(*this);
    }
};

struct Stmt
{
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};
struct StmtVisitor
{
    virtual ~StmtVisitor() = default;
    virtual void visitExpressionStmt(const Expression& stmt) = 0;
    virtual void visitPrintStmt(const Print& stmt) = 0;
    virtual void visitVarStmt(const Var& stmt) = 0;
};
struct Expression : Stmt 
{
    std::shared_ptr<Expr> expression;
    Expression(std::shared_ptr<Expr> expression):
    expression(expression){}
    void accept(StmtVisitor& visitor)
    {
        visitor.visitExpressionStmt(*this);
    }
};
struct Print : Stmt
{
    std::shared_ptr<Expr> printExpression;
    Print(std::shared_ptr<Expr> printExpression):
    printExpression(printExpression){}
    void accept(StmtVisitor& visitor)
    {
        visitor.visitPrintStmt(*this);
    }
};
struct Var : Stmt 
{
    std::shared_ptr<Token> token;
    std::shared_ptr<Expr> expression;
    Var(std::shared_ptr<Token> token, std::shared_ptr<Expr> expression):
    token(token), expression(expression){}
    void accept(StmtVisitor& visitor)
    {
        visitor.visitVarStmt(*this);
    }
};

struct parser
{
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
            if(match(TokenType::VAR) == true)
            {
                return varDeclaration();
            }
            return statement();
        }
        catch(RuntimeError)
        {
            synchronize();
            return nullptr;
        }
    }
    std::shared_ptr<Stmt> varDeclaration()
    {
        std::shared_ptr<Token> name = consume(TokenType::IDENTIFIER, "Expected variable name.");
        std::shared_ptr<Expr> initializer = nullptr;
        if(match(TokenType::EQUAL) == true)
        {
            initializer = expression();
        }
        consume(TokenType::SEMICOLON, "Expected a ';' after end of variable statement.");
        return std::make_shared<Var>(name, initializer);
    }
    std::shared_ptr<Stmt> statement()
    {
        if(match(TokenType::PRINT) == true)
        return printStatement();
        return expressionStatement();
    }
    std::shared_ptr<Stmt> printStatement()
    {
        std::shared_ptr<Expr> pexpression = expression();
        consume(TokenType::SEMICOLON, "Expected a semicolon after print statement.");
        return std::make_shared<Print>(pexpression);
    }
    std::shared_ptr<Stmt> expressionStatement()
    {
        std::shared_ptr<Expr> eexpression = expression();
        consume(TokenType::SEMICOLON, "Expected a semicolon after expression statement.");
        return std::make_shared<Expression>(eexpression);
    }
    std::shared_ptr<Expr> expression()
    {
        return assignment();
    }
    std::shared_ptr<Expr> assignment()
    {
        std::shared_ptr<Expr> expression = equality();
        {
            if(match(TokenType::EQUAL) == true)
            {
                std::shared_ptr<Token> token = previous();
                std::shared_ptr<Expr> value = assignment();
                std::shared_ptr<Variable> varExpr = std::dynamic_pointer_cast<Variable>(expression);
                if(varExpr != nullptr)
                {
                    std::shared_ptr<Token> name = varExpr->token;
                    return std::make_shared<Assign>(name, value);
                }
                error(token, "Invalid Assignment Target.");
            }
        }
        return expression;
    }
    std::shared_ptr<Expr> equality()
    {
        std::shared_ptr<Expr> left = comparison();
        while((match(TokenType::NOT_EQUAL)) || (match(TokenType::EQUAL_EQUAL)))
        {
            std::shared_ptr<Token> op = previous();
            std::shared_ptr<Expr> right = comparison();
            left = std::make_shared<Binary>(left, op, right);
        }
        return left;
    }
    std::shared_ptr<Expr> comparison()
    {
        std::shared_ptr<Expr> left = term();
        while(match(TokenType::GREATER_EQUAL) || match(TokenType::GREATER) || match(TokenType::LESS_EQUAL) || match(TokenType::LESS))
        {
            std::shared_ptr<Token> op = previous();
            std::shared_ptr<Expr> right = term();
            left = std::make_shared<Binary>(left, op, right);
        }
        return left;
    }
    std::shared_ptr<Expr> term()
    {
        std::shared_ptr<Expr> left = factor();
        while(match(TokenType::ADD) || match(TokenType::SUB))
        {
            std::shared_ptr<Token> op = previous();
            std::shared_ptr<Expr> right = factor();
            left = std::make_shared<Binary>(left, op, right);
        }
        return left;
    }
    std::shared_ptr<Expr> factor()
    {
        std::shared_ptr<Expr> left = unary();
        while(match(TokenType::MUL) || match(TokenType::DIV))
        {
            std::shared_ptr<Token> op = previous();
            std::shared_ptr<Expr> right = unary();
            left = std::make_shared<Binary>(left, op, right);
        }
        return left;
    }
    std::shared_ptr<Expr> unary()
    {
        if(match(TokenType::SUB) || match(TokenType::NOT))
        {
            std::shared_ptr<Token> op = previous();
            std::shared_ptr<Expr> right = unary();
            return std::make_shared<Unary>(op, right);
        }
        return primary();
    }
    std::shared_ptr<Expr> primary()
    {
        if(match(TokenType::FALSE)) return std::make_shared<Literal>(false);
        if(match(TokenType::TRUE)) return std::make_shared<Literal>(true);
        if(match(TokenType::NIL)) return std::make_shared<Literal>(nullptr);
        if(match(TokenType::STRING) || match(TokenType::NUMBER)) return std::make_shared<Literal>(previous()->literal);
        if (match(LEFT_PAREN)) 
        {
            std::shared_ptr<Expr> gexpression = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_shared<Grouping>(gexpression);
        }
        if(match(TokenType::IDENTIFIER)) return std::make_shared<Variable>(previous());
        throw error(peek(), "Expected an Expression.");
    }

    ParseError error(std::shared_ptr<Token> token, const std::string& message)
    {
        metal_error(token, message);
        return ParseError();
    }

    std::shared_ptr<Token> consume(TokenType type, const std::string& message)
    {
        if(check(type)) return advance();
        throw error(peek(), message);
    }

    bool match(TokenType type)
    {
        if(check(type) == true)
        {
            advance();
            return true;
        }
        return false;
    }

    bool check(TokenType type)
    {
        if(isAtEnd() == true)
        return false;
        if(peek()->type != type)
        return false;
        return true;
    }

    std::shared_ptr<Token> advance()
    {
        if(!isAtEnd())
        current++;
        return previous();
    }

    bool isAtEnd()
    {
        if(peek()->type == TokenType::EOF_TOKEN)
        {
            return true;
        }
        return false;
    }

    std::shared_ptr<Token> peek()
    {
        return tokens[current];
    }

    std::shared_ptr<Token> previous()
    {
        return tokens[current - 1];
    }
    void synchronize() 
    {
        advance();
        while (!isAtEnd()) 
        {
            if (previous()->type == TokenType::SEMICOLON) 
            return;
            switch (peek()->type) 
            {
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
            return;
            }
            advance();
        }
    }
};

#endif