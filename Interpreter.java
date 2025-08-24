import java.util.*;
class Interpreter implements Expr.Visitor<Object>, Stmt.Visitor<Void>
{
    private Environment environment = new Environment();
    public void interpret(List<Stmt> statements)
    {
        try
        {
            for(int i = 0; i < statements.size(); i++)
            {
                execute(statements.get(i));
            }
        }
        catch(RuntimeError error)
        {
            Metal.runtimeError(error);
        }
    }
    
    public void execute(Stmt statement)
    {
        statement.accept(this);
    }
    
    public Void visitVarStmt(Stmt.Var stmt)
    {
        Object value = null;
        if(stmt.initializer != null)
        value = evaluate(stmt.initializer);
        environment.define(stmt.name.lexeme, value);
        return null;
    }
    
    public Object visitVariableExpr(Expr.Variable expr)
    {
        return environment.get(expr.name);
    }
    
    public Object visitAssignExpr(Expr.Assign expr)
    {
        Object value = evaluate(expr.value);
        environment.assign(expr.name, value);
        return value;
    }
    public Void visitExpressionStmt(Stmt.Expression stmt)
    {
        evaluate(stmt.expression);
        return null;
    }
    
    public Void visitPrintStmt(Stmt.Print stmt)
    {
        Object value = evaluate(stmt.expression);
        System.out.println(stringify(value));
        return null;
    }
    
    public Object visitLiteralExpr(Expr.Literal expr)
    {
        return expr.value;
    }
    
    public Object visitGroupingExpr(Expr.Grouping expr)
    {
        return evaluate(expr.expression);
    }
    
    public Object visitUnaryExpr(Expr.Unary expr)
    {
        Object right = evaluate(expr.left);
        switch(expr.operator.type)
        {
            case TokenType.BANG:
            return !isTrue(right);
            case TokenType.MINUS:
            {
                checkNumberOperand(expr.operator, right);
                return -(double)right;
            }
        }
        return null;
    }
    
    public Object visitBinaryExpr(Expr.Binary expr)
    {
        Object left = evaluate(expr.left);
        Object right = evaluate(expr.right);
        
        switch(expr.operator.type)
        {
            case TokenType.PLUS:
            {
                if(left instanceof Double && right instanceof Double)
                return (double)left + (double)right;
                if(left instanceof String && right instanceof String)
                return (String)left + (String)right;
                throw new RuntimeError(expr.operator, "Operands must be 2 numbers or 2 strings.");
            }
            case TokenType.MINUS:
            {
                checkNumberOperand(expr.operator, left, right);
                return (double)left - (double)right;
            }
            case TokenType.SLASH:
            {
                checkNumberOperand(expr.operator, left, right);
                return (double)left / (double)right;
            }
            case TokenType.STAR:
            {
                checkNumberOperand(expr.operator, left, right);
                return (double)left * (double)right;
            }
            case TokenType.GREATER:
            {
                checkNumberOperand(expr.operator, left, right);
                return (double)left > (double)right;
            }
            case TokenType.LESS:
            {
                checkNumberOperand(expr.operator, left, right);
                return (double)left < (double)right;
            }
            case TokenType.GREATER_EQUAL:
            {
                checkNumberOperand(expr.operator, left, right);
                return (double)left >= (double)right;
            }
            case TokenType.LESS_EQUAL:
            {
                checkNumberOperand(expr.operator, left, right);
                return (double)left <= (double)right;
            }
        }
        return null;
    }
    
    public Object evaluate(Expr expr)
    {
        return expr.accept(this);
    }
    
    public boolean isTrue(Object object)
    {
        if(object == null)return false;
        if(object instanceof Boolean == true) return (boolean)object;
        return true;
    }
    
    public boolean isEqual(Object a, Object b)
    {
        if(a == null && b == null)return true;
        if(a == null)return false;
        return a.equals(b);
    }
    
    public boolean checkNumberOperand(Token operator, Object right)
    {
        if(right instanceof Double)
        return true;
        throw new RuntimeError(operator, "Operand must be a number");
    }
    
    public boolean checkNumberOperand(Token operator, Object left, Object right)
    {
        if(left instanceof Double && right instanceof Double) 
        return true;
        throw new RuntimeError(operator, "Operands must be numbers");
    }
    
    public String stringify(Object object)
    {
        if(object == null) return "nil";
        if(object instanceof Double)
        {
            String text = object.toString();
            if(text.endsWith(".0"))
            text = text.substring(0, text.length() - 2);
            return text;
        }
        return object.toString();
    }
}
