class Token
{
    final int line;
    final String lexeme;
    final TokenType token;
    final Object literal;
    
    Token(int line, String lexeme, TokenType token, Object literal)
    {
        this.line = line;
        this.lexeme = lexeme;
        this.token = token;
        this.literal = literal;
    }

    public String toString()
    {
        return token + " " + lexeme + " " + literal;
    }
}
