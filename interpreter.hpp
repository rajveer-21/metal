#ifndef interpreter_hpp
#define interpreter_hpp
#include "parser.hpp"
struct Environment;
void metal_runtime_error(const RuntimeError& error);
struct interpreter : public ExprVisitor, public StmtVisitor
{
    std::shared_ptr<Environment> environment;
    interpreter(): 
    environment(std::make_shared<Environment>()) {}
    void interpret(std::vector<std::shared_ptr<Stmt>> statements) 
    {
        try 
        {
            for(int i = 0; i < statements.size(); i++)
            {
                execute(statements[i]);
            }
        } 
        catch (const RuntimeError& error) 
        {
            metal_runtime_error(error); 
        }
    }
    void execute(std::shared_ptr<Stmt> stmt)
    {
        if (stmt == nullptr) return;
        stmt->accept(*this);
    }
    std::string stringify(const std::any& value)
    {
        if(value.type() == typeid(nullptr))
        return "nil";
        if(value.type() == typeid(double))
        {
            std::string text = std::to_string(std::any_cast<double>(value));
            if(text.size() > 2)
            {
                int dot_pos = text.find('.');
                std::string fraction = text.substr(dot_pos + 1);
                double num = std::stod(fraction);
                if(num == 0)
                text = text.substr(0, dot_pos);
            }
            return text;
        }
        if(value.type() == typeid(bool))
        {
            bool value1 = std::any_cast<bool>(value);
            if(value1 == true)
            return "true";
            return "false";
        }
        if (value.type() == typeid(std::string)) 
        {
            return std::any_cast<std::string>(value);
        }
        return "????";
    }
    std::any visitBinaryExpr(const Binary& expr)
    {
        std::any left = evaluate(expr.left);
        std::any right = evaluate(expr.right);
        switch(expr.op->type)
        {
            case TokenType::ADD:
            {
                if(left.type() == typeid(double) && right.type() == typeid(double))
                return std::any_cast<double>(left) + std::any_cast<double>(right);
                if(left.type() == typeid(std::string) && right.type() == typeid(std::string))
                return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
                throw RuntimeError(expr.op, "Operands must be either strings or numbers.");
            }
            case TokenType::SUB:
            checkNumberOperands(left, right, expr.op);
            return std::any_cast<double>(left) - std::any_cast<double>(right);
            case TokenType::MUL:
            checkNumberOperands(left, right, expr.op);
            return std::any_cast<double>(left) * std::any_cast<double>(right);
            case TokenType::DIV:
            checkNumberOperands(left, right, expr.op);
            return std::any_cast<double>(left) / std::any_cast<double>(right);
            case TokenType::GREATER:
            checkNumberOperands(left, right, expr.op);
            return std::any_cast<double>(left) > std::any_cast<double>(right);
            case TokenType::GREATER_EQUAL:
            checkNumberOperands(left, right, expr.op);
            return std::any_cast<double>(left) >= std::any_cast<double>(right);
            case TokenType::LESS:
            checkNumberOperands(left, right, expr.op);
            return std::any_cast<double>(left) < std::any_cast<double>(right);
            case TokenType::LESS_EQUAL:
            checkNumberOperands(left, right, expr.op);
            return std::any_cast<double>(left) <= std::any_cast<double>(right);
            case TokenType::NOT_EQUAL:
            return !isEqual(left, right);
            case TokenType::EQUAL_EQUAL:
            return isEqual(left, right);
            default:
            throw RuntimeError(expr.op, "Unexpected binary operator.");
        }
    }
    std::any visitUnaryExpr(const Unary& expr)
    {
        std::any right = evaluate(expr.right);
        switch(expr.op->type)
        {
            case TokenType::SUB:
            {
                checkNumberOperand(expr.op, right);
                return -std::any_cast<double>(right);
            }
            case TokenType::NOT:
            {
                return !isTrue(right);
            }
        }
        throw RuntimeError(expr.op, "Unexpected unary operator.");
    }
    std::any visitLiteralExpr(const Literal& expr)
    {
        return expr.value;
    }
    std::any visitGroupingExpr(const Grouping& expr)
    {
        return evaluate(expr.expression);
    }
    std::any visitLogicalExpr(const Logical& expr)
    {
        std::any value = evaluate(expr.left);
        if(expr.op->type == TokenType::OR)
        {
            if(isTrue(value)) return value;
        }
        else if(expr.op->type == TokenType::AND)
        {
            if(!isTrue(value)) return value;
        }
        return evaluate(expr.right);
    }
    std::any visitVariableExpr(const Variable& expr)
    {
        return environment->get(expr.token);
    }
    std::any visitAssignExpr(const Assign& expr)
    {
        std::any value = evaluate(expr.expression);
        environment->assign(expr.token, value);
        return value;
    }
    void visitExpressionStmt(const Expression& stmt)
    {
        evaluate(stmt.expression);
        return;
    }
    void visitIfStmt(const If& stmt) 
    {
        if(isTrue(evaluate(stmt.condition)))
        execute(stmt.thenBranch);
        else 
        {
            if(stmt.elseBranch != nullptr)
            execute(stmt.elseBranch);
        }
    }
    void visitVarStmt(const Var& stmt)
    {
        std::any value = std::make_any<std::nullptr_t>(nullptr); 
        if(stmt.expression != nullptr)
        {
            value = evaluate(stmt.expression);
        }
        environment->define(stmt.token->lexeme, value);
        return;
    }
    void visitPrintStmt(const Print& stmt)
    {
        std::any value = evaluate(stmt.printExpression);
        std::cout << stringify(value) << std::endl;
        return;
    }
    void visitBlockStmt(const Block& stmt)
    {
        std::shared_ptr<Environment> new_environment = 
        std::make_shared<Environment>(this->environment);
        executeBlock(stmt.statements, new_environment);
        return;
    }
    void visitWhileStmt(const While& stmt)
    {
        while(isTrue(evaluate(stmt.condition)) == true)
        {
            execute(stmt.body);
        }
    }
    void executeBlock(std::vector<std::shared_ptr<Stmt>> statements, std::shared_ptr<Environment> environment)
    {
        std::shared_ptr<Environment> previous = this->environment;
        this->environment = environment;
        try 
        {
            for(int i = 0; i < statements.size(); i++)
            execute(statements[i]);
        }
        catch(const RuntimeError& error)
        {
            this->environment = previous;
            throw;
        }
        this->environment = previous;
    }
    std::any evaluate(std::shared_ptr<Expr> expr)
    {
        return expr->accept(*this);
    }
    bool isTrue(const std::any& expression)
    {
        if(expression.type() == typeid(nullptr))
        return false;
        if(expression.type() == typeid(bool))
        return std::any_cast<bool>(expression);
        return true;
    }
    bool isEqual(const std::any& a, const std::any& b) 
    {
        bool a_is_nil = (a.type() == typeid(nullptr));
        bool b_is_nil = (b.type() == typeid(nullptr));
        if (a_is_nil && b_is_nil) 
        return true;
        if (a_is_nil) 
        return false;
        if (a.type() != b.type())
        return false;
        if (a.type() == typeid(double))
        return std::any_cast<double>(a) == std::any_cast<double>(b);
        if (a.type() == typeid(bool))
        return std::any_cast<bool>(a) == std::any_cast<bool>(b);
        if (a.type() == typeid(std::string))
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
        return false;
    }
    void checkNumberOperand(std::shared_ptr<Token> token, const std::any& op)
    {
        if(op.type() == typeid(double))
        return;
        throw RuntimeError(token, "Operand must be a number.");
    }
    void checkNumberOperands(const std::any& operand1, const std::any& operand2, std::shared_ptr<Token> op)
    {
        if(operand1.type() == typeid(double) && operand2.type() == typeid(double))
        return;
        throw RuntimeError(op, "Operands must be numbers.");
    }
};
#endif