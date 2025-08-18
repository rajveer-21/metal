abstract class Expr
{
    interface Visitor<R>
    {
        R visitBinaryExpr(Binary Expr);
        R visitGroupingExpr(Grouping Expr);
        R visitUnaryExpr(Unary Expr);
        R visitLiteralExpr(Literal Expr);
    }
    abstract <R> R accept(Visitor <R> visitor);
    static class Binary extends Expr
    {
        Binary(Expr left, Token operator, Expr right)
        {
            this.left = left;
            this.operator = operator;
            this.right = right;
        }
        final Expr left;
        final Token operator;
        final Expr right;
        <R> R accept(Visitor <R> visitor)
        {
            return visitor.visitBinaryExpr(this);
        }
    }
    static class Unary extends Expr
    {
        Unary(Token operator, Expr left)
        {
            this.operator = operator;
            this.left = left;
        }
        final Token operator;
        final Expr left;
        <R> R accept(Visitor <R> visitor)
        {
            return visitor.visitUnaryExpr(this);
        }
    }
    static class Literal extends Expr
    {
        Literal(Object value)
        {
            this.value = value;
        }
        final Object value;
        <R> R accept(Visitor<R> visitor)
        {
            return visitor.visitLiteralExpr(this);
        }
    }
    static class Grouping extends Expr
    {
        Grouping(Expr expression)
        {
            this.expression = expression;
        }
        final Expr expression;
        <R> R accept(Visitor <R> visitor)
        {
            return visitor.visitGroupingExpr(this);
        }
    }
}