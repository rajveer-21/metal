#ifndef ExpressionsStatementsEnvironments_hpp
#define ExpressionsStatementsEnvironments_hpp
#include "lexer.hpp"

struct Token;
struct Expr
{
    struct Visitor;
    virtual ~Expr() = default;
    virtual std::any accept(Visitor& visitor) = 0;
    struct Visitor
    {
        virtual ~Visitor() = default;
        virtual std::any visitUnaryExpr(class UnaryExpr& unary) = 0;
        virtual std::any visitBinaryExpr(class BinaryExpr& binary) = 0;
        virtual std::any visitAssignExpr(class AssignExpr& assign) = 0;
        virtual std::any visitLiteralExpr(class LiteralExpr& literal) = 0;
        virtual std::any visitLogicalExpr(class LogicalExpr& logical) = 0;
        virtual std::any visitGroupingExpr(class GroupingExpr& grouping) = 0;
        virtual std::any visitVariableExpr(class VariableExpr& variable) = 0;
    };
};

using ExprPtr = std::shared_ptr<Expr>;
struct BinaryExpr : public Expr
{
    Token* token;
    ExprPtr operand1;
    ExprPtr operand2;
    BinaryExpr(Token* token, ExprPtr operand1, ExprPtr operand2):
    token(token), operand1(operand1), operand2(operand2){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitBinaryExpr(*this);
    }
};
struct UnaryExpr : public Expr
{
    Token* token;
    ExprPtr operand;
    UnaryExpr(Token* token, ExprPtr operand):
    token(token), operand(operand){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitUnaryExpr(*this);
    }
};
struct GroupingExpr : public Expr
{
    ExprPtr groupingExpr;
    GroupingExpr(ExprPtr groupingExpr):
    groupingExpr(groupingExpr){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitGroupingExpr(*this);
    }
};
struct LiteralExpr : public Expr
{
    std::any value;
    LiteralExpr(std::any value):
    value(value){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitLiteralExpr(*this);
    }
};
struct LogicalExpr : public Expr
{
    ExprPtr operand1;
    ExprPtr operand2;
    Token* token;
    LogicalExpr(ExprPtr operand1, ExprPtr operand2, Token* token):
    operand1(operand1), operand2(operand2), token(token){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitLogicalExpr(*this);
    }
};
struct AssignExpr : public Expr
{
    Token* variable_name;
    ExprPtr assignment_value;
    AssignExpr(Token* variable_name, ExprPtr assignment_value):
    variable_name(variable_name), assignment_value(assignment_value){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitAssignExpr(*this);
    }
};
struct VariableExpr : public Expr
{
    Token* name;
    VariableExpr(Token* name):
    name(name){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitVariableExpr(*this);
    }
};

struct Expr;
struct Stmt
{
    struct Visitor;
    virtual ~Stmt() = default;
    virtual std::any accept(Visitor& visitor) = 0;
    struct Visitor
    {
        virtual ~Visitor() = default;
        virtual std::any visitVariableStmt(class VariableStmt& variable) = 0;
        virtual std::any visitIfStmt(class IfStmt& ifStmt) = 0;
        virtual std::any visitBlockStmt(class BlockStmt& block) = 0;
        virtual std::any visitPrintStmt(class PrintStmt& print) = 0;
        virtual std::any visitWhileStmt(class WhileStmt& whileStmt) = 0;
        virtual std::any visitExpressionStmt(class ExpressionStmt& expression) = 0;
    };
};
using StmtPtr = std::shared_ptr<Stmt>;
struct VariableStmt : public Stmt
{
    Token* token;
    std::shared_ptr<Expr> initializer;
    VariableStmt(Token* token, std::shared_ptr<Expr> initializer):
    token(token), initializer(initializer){};
    std::any accept(Visitor& visitor)
    {
        return visitor.visitVariableStmt(*this);
    }
};
struct IfStmt : public Stmt
{
    std::shared_ptr<Expr> condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;
    IfStmt(std::shared_ptr<Expr> condition, StmtPtr thenBranch, StmtPtr elseBranch):
    condition(condition), thenBranch(thenBranch), elseBranch(elseBranch){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitIfStmt(*this);
    }
};
struct BlockStmt : public Stmt
{
    std::vector<StmtPtr> statements;
    BlockStmt(std::vector<StmtPtr> statements):
    statements(statements){};
    std::any accept(Visitor& visitor)
    {
        return visitor.visitBlockStmt(*this);
    }
};
struct PrintStmt : public Stmt
{
    std::shared_ptr<Expr> printval;
    PrintStmt(std::shared_ptr<Expr> printval):
    printval(printval){};
    std::any accept(Visitor& visitor)
    {
        return visitor.visitPrintStmt(*this);
    }
};
struct ExpressionStmt : public Stmt
{
    std::shared_ptr<Expr> expression;
    ExpressionStmt(std::shared_ptr<Expr> expression):
    expression(expression){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitExpressionStmt(*this);
    }
};
struct WhileStmt : public Stmt
{
    std::shared_ptr<Expr> condition;
    StmtPtr body;
    WhileStmt(std::shared_ptr<Expr> condition, StmtPtr body):
    condition(condition), body(body){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitWhileStmt(*this);
    }
};

struct RuntimeError : std::runtime_error
{
    Token* token;
    RuntimeError(Token* token, const std::string& message):
    token(token), std::runtime_error(message){}
};

struct Environment
{
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, std::any> values;
    Environment():
    enclosing(nullptr){}
    Environment(std::shared_ptr<Environment> enclosing):
    enclosing(enclosing){};

    void define(const std::string& name, const std::any& value)
    {
        values[name] = value;
    }
    void assign(Token* name, std::any& value)
    {   
        if(values.find(name->lexeme) != values.end())
        {
            values[name->lexeme] = value;   
        }
        if(enclosing != nullptr)
        {
            enclosing->assign(name, value);
            return;
        }
        throw RuntimeError(name, "RUNTIME ERROR : Trying to assign to undefined variable.");
    }
    std::any get(Token* name)
    {
        auto search = values.find(name->lexeme);
        if(search != values.end()) return search->second;
        if(enclosing != nullptr) return enclosing->get(name);
        throw RuntimeError(name, "RUNTIME ERROR : Trying to access an undefined variable.");
    }
};

#endif