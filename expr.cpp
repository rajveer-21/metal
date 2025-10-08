#include <memory>
#include <vector>
#include <any>

struct Token;
struct Expr
{
    struct Visitor;
    virtual ~Expr() = default;
    virtual std::any accept(Visitor& visitor) = 0;
    struct Visitor
    {
        virtual ~Visitor() = default;
        virtual std::any visitUnaryExpr(class Unary& unary) = 0;
        virtual std::any visitBinaryExpr(class Binary& binary) = 0;
        virtual std::any visitAssignExpr(class Assign& assign) = 0;
        virtual std::any visitLiteralExpr(class Literal& literal) = 0;
        virtual std::any visitLogicalExpr(class Logical& logical) = 0;
        virtual std::any visitGroupingExpr(class Grouping& grouping) = 0;
        virtual std::any visitVariableExpr(class Variable& variable) = 0;
    };
};

using ExprPtr = std::shared_ptr<Expr>;
struct Binary : public Expr
{
    Token* token;
    ExprPtr operand1;
    ExprPtr operand2;
    Binary(Token* token, ExprPtr operand1, ExprPtr operand2):
    token(token), operand1(operand1), operand2(operand2){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitBinaryExpr(*this);
    }
};
struct Unary : public Expr
{
    Token* token;
    ExprPtr operand;
    Unary(Token* token, ExprPtr operand):
    token(token), operand(operand){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitUnaryExpr(*this);
    }
};
struct Grouping : public Expr
{
    ExprPtr groupingExpr;
    Grouping(ExprPtr groupingExpr):
    groupingExpr(groupingExpr){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitGroupingExpr(*this);
    }
};
struct Literal : public Expr
{
    std::any value;
    Literal(std::any value):
    value(value){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitLiteralExpr(*this);
    }
};
struct Logical : public Expr
{
    ExprPtr operand1;
    ExprPtr operand2;
    Token* token;
    Logical(ExprPtr operand1, ExprPtr operand2, Token* token):
    operand1(operand1), operand2(operand2), token(token){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitLogicalExpr(*this);
    }
};
struct Assign : public Expr
{
    Token* variable_name;
    ExprPtr assignment_value;
    Assign(Token* variable_name, ExprPtr assignment_value):
    variable_name(variable_name), assignment_value(assignment_value){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitAssignExpr(*this);
    }
};
struct Variable : public Expr
{
    Token* name;
    Variable(Token* name):
    name(name){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitVariableExpr(*this);
    }
};