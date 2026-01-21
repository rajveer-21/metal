#ifndef parser_hpp
#define parser_hpp
#include "scanner.hpp"
struct Expr;
struct Binary;
struct Assign;
struct Unary;
struct Grouping;
struct Literal;
struct Logical;
struct Variable;
struct ExprVisitor;
struct ArrayDefine;
struct ArrayAccess;
struct ArrayAssign;
struct Stmt;
struct Print;
struct Block;
struct While;
struct Var;
struct If;
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
    std::shared_ptr<Environment> enclosing;
    Environment():
    enclosing(nullptr){}
    Environment(std::shared_ptr<Environment> enclosing):
    enclosing(enclosing){}
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
        if(enclosing != nullptr)
        return enclosing->get(token);
        else
        throw RuntimeError(token, "Undefined Variable");
    }
    void assign(const std::shared_ptr<Token>& token, std::any value)
    {
        if(values.count(token->lexeme)) 
        {
            values[token->lexeme] = value;
            return;
        }
        if(enclosing != nullptr)
        {
            enclosing->assign(token, value);
            return;
        }
        throw RuntimeError(token, "Undefined variable '" + token->lexeme + "'.");
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
    virtual std::any visitLogicalExpr(const Logical& expr) = 0;
    /*
    virtual std::any visitArrayDefine(const ArrayDefine& expr)  = 0;
    virtual std::any visitArrayAssign(const ArrayAssign& expr)  = 0;
    virtual std::any visitArrayAccess(const ArrayAccess& expr)  = 0;
    */
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
/*
struct ArrayDefine : Expr
{
    std::vector<std::shared_ptr<Expr>> elements;
    ArrayDefine(std::vector<std::shared_ptr<Expr>> elements):
    elements(elements){}
    std::any accept(ExprVisitor& visitor)
    {
        return visitor.visitArrayDefine(*this);
    }
};
struct ArrayAccess : Expr
{
    std::shared_ptr<Expr> array;
    std::shared_ptr<Expr> index;
    ArrayAccess(std::shared_ptr<Expr> array, std::shared_ptr<Expr> index):
    array(array), index(index){};
    std::any accept(ExprVisitor& visitor)
    {
        return visitor.visitArrayAccess(*this);
    }
};
struct ArrayAssign : Expr
{
    std::shared_ptr<Expr> array;
    std::shared_ptr<Expr> index;
    std::shared_ptr<Expr> value;
    ArrayAssign(std::shared_ptr<Expr> array, std::shared_ptr<Expr> index, std::shared_ptr<Expr> value):
    array(array), index(index), value(value){}
    std::any accept(ExprVisitor& visitor)
    {
        return visitor.visitArrayAssign(*this);
    }
};
*/
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
struct Logical : Expr 
{
    std::shared_ptr<Expr> left ;
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;
    Logical(std::shared_ptr<Expr> left, std::shared_ptr<Token> op, std::shared_ptr<Expr> right):
    left(left), right(right), op(op){}
    std::any accept(ExprVisitor& visitor)
    {
        return visitor.visitLogicalExpr(*this);
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
    virtual void visitBlockStmt(const Block& stmt) = 0;
    virtual void visitWhileStmt(const While& stmt) = 0;
    virtual void visitVarStmt(const Var& stmt) = 0;
    virtual void visitIfStmt(const If& stmt) = 0;
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
struct While : Stmt 
{
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
    While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body):
    condition(condition), body(body){}
    void accept(StmtVisitor& visitor)
    {
        visitor.visitWhileStmt(*this);
    }
};
struct Block : Stmt 
{
    std::vector<std::shared_ptr<Stmt>> statements;
    Block(std::vector<std::shared_ptr<Stmt>> statements):
    statements(statements){}
    void accept(StmtVisitor& visitor)
    {
        visitor.visitBlockStmt(*this);
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
struct If : Stmt 
{
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
    If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch):
    condition(condition), thenBranch(thenBranch), elseBranch(elseBranch){}
    void accept(StmtVisitor& visitor)
    {
        visitor.visitIfStmt(*this);
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
        catch(ParseError)
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
        if(match(TokenType::IF)) return ifStatement();
        if(match(TokenType::FOR)) return forStatement();
        if(match(TokenType::PRINT) == true)
        return printStatement();
        if(match(TokenType::WHILE) == true)
        return whileStatement();
        if(match(TokenType::LEFT_BRACE) == true)
        return std::make_shared<Block>(block());
        return expressionStatement();
    }
    std::shared_ptr<Stmt> ifStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expected a ')' after if keyword.");
        std::shared_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition."); 
        std::shared_ptr<Stmt> thenBranch = statement();
        std::shared_ptr<Stmt> elseBranch = nullptr;
        if (match(TokenType::ELSE)) 
        {
            elseBranch = statement();
        }
        return std::make_shared<If>(condition, thenBranch, elseBranch);
    }
    std::shared_ptr<Stmt> printStatement()
    {
        std::shared_ptr<Expr> pexpression = expression();
        consume(TokenType::SEMICOLON, "Expected a semicolon after print statement.");
        return std::make_shared<Print>(pexpression);
    }
    std::shared_ptr<Stmt> forStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
        std::shared_ptr<Stmt> initializer;
        if (match(TokenType::SEMICOLON)) 
        initializer = nullptr;  
        else if (match(TokenType::VAR)) 
        initializer = varDeclaration();  
        else 
        initializer = expressionStatement();
        std::shared_ptr<Expr> condition = nullptr;
        if (!check(TokenType::SEMICOLON)) 
        condition = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
        std::shared_ptr<Expr> increment = nullptr;
        if (!check(TokenType::RIGHT_PAREN))
        increment = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
        std::shared_ptr<Stmt> body = statement();
        if (increment != nullptr) 
        {
            std::vector<std::shared_ptr<Stmt>> statements;
            statements.push_back(body); 
            statements.push_back(std::make_shared<Expression>(increment));
            body = std::make_shared<Block>(std::move(statements));
        } 
        if (condition == nullptr) 
        condition = std::make_shared<Literal>(true);
        body = std::make_shared<While>(condition, body);
        if (initializer != nullptr) 
        {
            std::vector<std::shared_ptr<Stmt>> statements;
            statements.push_back(initializer);
            statements.push_back(body);
            body = std::make_shared<Block>(std::move(statements));
        }
        return body;
    }
    std::shared_ptr<Stmt> whileStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expected a '(' after while keyword.");
        std::shared_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expected a ')' after end of while condition.");
        std::shared_ptr<Stmt> body = statement();
        return std::make_shared<While>(condition, body);
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
        std::shared_ptr<Expr> expression = orExpr();
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
                /*
                std::shared_ptr<ArrayAccess> arrA = std::dynamic_pointer_cast<ArrayAccess>(expression);
                if(arrA != nullptr)
                {
                    return std::make_shared<ArrayAssign>(arrA->array, arrA->index, value);
                }
                throw error(token, "Invalid Assignment Target.");
                */
            }
        }
        return expression;
    }
    std::shared_ptr<Expr> orExpr() 
    {
        std::shared_ptr<Expr> left = andExpr();
        while(match(TokenType::OR) == true)
        {
            std::shared_ptr<Token> op = previous();
            std::shared_ptr<Expr> right = andExpr();
            left = std::make_shared<Logical>(left, op, right); 
        }
        return left;
    }
    std::shared_ptr<Expr> andExpr() 
    {
        std::shared_ptr<Expr> left = equality();
        while(match(TokenType::AND) == true)
        {
            std::shared_ptr<Token> op = previous();
            std::shared_ptr<Expr> right = equality();
            left = std::make_shared<Logical>(left, op, right);
        }
        return left;
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
    /*
    std::shared_ptr<Expr> arrayfn()
    {
        std::shared_ptr<Expr> expr = primary();
        while(match(TokenType::BRACKET_ST))
        {
            expr = finish_array_operation(expr);
        }
        return expr;
    }
    std::shared_ptr<Expr> finish_array_operation(std::shared_ptr<Expr> array)
    {
        std::shared_ptr<Expr> index = expression();
        consume(TokenType::BRACKET_END, "ERROR : Expected ']' after end of array.");
        return std::make_shared<ArrayAccess>(array, index);
    }
    */
    std::shared_ptr<Expr> primary()
    {
        if(match(TokenType::FALSE)) return std::make_shared<Literal>(false);
        if(match(TokenType::TRUE)) return std::make_shared<Literal>(true);
        if(match(TokenType::NIL)) return std::make_shared<Literal>(nullptr);
        if(match(TokenType::STRING) || match(TokenType::NUMBER)) return std::make_shared<Literal>(previous()->literal);
        if(match(TokenType::LEFT_PAREN)) 
        {
            std::shared_ptr<Expr> gexpression = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_shared<Grouping>(gexpression);
        }
        /*
        if(match(TokenType::BRACKET_ST))
        {
            return arraydefine();
        }
        */
        if(match(TokenType::IDENTIFIER)) return std::make_shared<Variable>(previous());
        throw error(peek(), "Expected an Expression.");
    }
    /*
    std::shared_ptr<Expr> arraydefine()
    {
        std::vector<std::shared_ptr<Expr>> array_elements;
        if(!check(TokenType::BRACKET_END))
        {
            do 
            {
                array_elements.push_back(expression());
            }
            while(match(TokenType::COMMA));
        }
        consume(TokenType::BRACKET_END, "ERROR! = Expected a ']' at the end of an array.");
        return std::make_shared<ArrayDefine>(std::move(array_elements));
    }
    */
    std::vector<std::shared_ptr<Stmt>> block()
    {
        std::vector<std::shared_ptr<Stmt>> statements;
        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        statements.push_back(declaration());
        consume(TokenType::RIGHT_BRACE, "Expected a '}' at end of a block.");
        return statements;
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