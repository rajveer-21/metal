#include <memory>
#include <vector>
#include <any>

struct Token;
struct Expr;
struct Stmt
{
    struct Visitor;
    virtual ~Stmt() = default;
    virtual std::any accept(Visitor& visitor) = 0;
    struct Visitor
    {
        virtual std::any visitVariableStmt(Variable& variable) = 0;
        virtual std::any visitIfStmt(If& ifStmt) = 0;
        virtual std::any visitBlockStmt(Block& block) = 0;
        virtual std::any visitPrintStmt(Print& print) = 0;
        virtual std::any visitWhileStmt(While& whileStmt) = 0;
        virtual std::any visitExpressionStmt(Expression& expression) = 0;
        virtual ~Visitor() = default;
    };
};
using StmtPtr = std::shared_ptr<Stmt>;
struct Variable : public Stmt
{
    Token* token;
    std::shared_ptr<Expr> initializer;
    Variable(Token* token, std::shared_ptr<Expr> initializer):
    token(token), initializer(initializer){};
    std::any accept(Visitor& visitor)
    {
        return visitor.visitVariableStmt(*this);
    }
};
struct If : public Stmt
{
    std::shared_ptr<Expr> condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;
    If(std::shared_ptr<Expr> condition, StmtPtr thenBranch, StmtPtr elseBranch):
    condition(condition), thenBranch(thenBranch), elseBranch(elseBranch){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitIfStmt(*this);
    }
};
struct Block : public Stmt
{
    std::vector<StmtPtr> statements;
    Block(std::vector<StmtPtr> statements):
    statements(statements){};
    std::any accept(Visitor& visitor)
    {
        return visitor.visitBlockStmt(*this);
    }
};
struct Print : public Stmt
{
    std::shared_ptr<Expr> printval;
    Print(std::shared_ptr<Expr> printval):
    printval(printval){};
    std::any accept(Visitor& visitor)
    {
        return visitor.visitPrintStmt(*this);
    }
};
struct Expression : public Stmt
{
    std::shared_ptr<Expr> expression;
    Expression(std::shared_ptr<Expr> expression):
    expression(expression){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitExpressionStmt(*this);
    }
};
struct While : public Stmt
{
    std::shared_ptr<Expr> condition;
    StmtPtr body;
    While(std::shared_ptr<Expr> condition, StmtPtr body):
    condition(condition), body(body){}
    std::any accept(Visitor& visitor)
    {
        return visitor.visitWhileStmt(*this);
    }
};