class PrettyPrinter implements Expr.Visitor<String>
{
    String print(Expr expr)
    {
        return expr.accept(this);
    }
    
    public String visitBinaryExpr(Expr.Binary expr)
    {
        return parenthesize(expr.operator.lexeme, expr.left, expr.right);
    }
    
    public String visitGroupingExpr(Expr.Grouping expr)
    {
        return parenthesize("group", expr.expression);
    }
    
    public String visitLiteralExpr(Expr.Literal expr)
    {
        if(expr.value == null) return "nil";
        return expr.value.toString();
    }
    
    public String visitUnaryExpr(Expr.Unary expr)
    {
        return parenthesize(expr.operator.lexeme, expr.left);
    }
    
    private String parenthesize(String name, Expr... exprs)
    {
        StringBuilder builder = new StringBuilder();
        builder.append("(").append(name);
        for(Expr expr : exprs)
        {
            builder.append(" ");
            builder.append(expr.accept(this));
        }
        builder.append(" ");
        return builder.toString();
    }
}