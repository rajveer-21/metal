import java.io.*;
import java.util.*;

class Scanner
{
    private static int line = 1;
    private static int start = 0;
    private static int current = 0;
    private final String source;
    private final ArrayList<Token> tokens = new ArrayList<>();
    private static final Map<String, TokenType> keywords;
    static 
    {
        keywords = new HashMap<>();
        keywords.put("and",    TokenType.AND);
        keywords.put("class",  TokenType.CLASS);
        keywords.put("else",   TokenType.ELSE);
        keywords.put("false",  TokenType.FALSE);
        keywords.put("for",    TokenType.FOR);
        keywords.put("fun",    TokenType.FUN);
        keywords.put("if",     TokenType.IF);
        keywords.put("nil",    TokenType.NIL);
        keywords.put("or",     TokenType.OR);
        keywords.put("print",  TokenType.PRINT);
        keywords.put("return", TokenType.RETURN);
        keywords.put("super",  TokenType.SUPER);
        keywords.put("this",   TokenType.THIS);
        keywords.put("true",   TokenType.TRUE);
        keywords.put("var",    TokenType.VAR);
        keywords.put("while",  TokenType.WHILE);
    }
    Scanner(String source)
    {
        this.source = source;
    }
    
    public List<Token> scanTokens()
    {
        while(isAtEnd() == false)
        {
            start = current;
            scanToken();
        }
        tokens.add(new Token(TokenType.EOF, "", null, line));
        return tokens;
    }
    
    public void scanToken()
    {
        char ch = source.charAt(current);
        switch(ch)
        {
            case '(' : addToken(TokenType.LEFT_PAREN); break;
            case ')' : addToken(TokenType.RIGHT_PAREN); break;
            case '{' : addToken(TokenType.LEFT_BRACE); break;
            case '}' : addToken(TokenType.RIGHT_BRACE); break;
            case '+' : addToken(TokenType.PLUS); break;
            case '-' : addToken(TokenType.MINUS); break;
            case '*' : addToken(TokenType.STAR); break;
            case ';' : addToken(TokenType.SEMICOLON); break;
            case '.' : addToken(TokenType.DOT); break;
            case ',' : addToken(TokenType.COMMA); break;
            case '!' :
            {
                if(match('=') == true) 
                addToken(TokenType.BANG_EQUAL);
                else
                addToken(TokenType.BANG);
                break;
            }
            case '=' :
            {
                if(match('=') == true)
                addToken(TokenType.EQUAL_EQUAL);
                else
                addToken(TokenType.EQUAL);
                break;
            }
            case '>':
            {
                if(match('=') == true)
                addToken(TokenType.GREATER_EQUAL);
                else
                addToken(TokenType.GREATER);
                break;
            }
            case '<':
            {
                if(match('=') == true)
                addToken(TokenType.LESS_EQUAL);
                else
                addToken(TokenType.LESS);
                break;
            }
            case '"' : string(); break;
            case ' ': break;
            case '\t': break;
            case '\r': break;
            case '\n': line++; break;
            default: 
            {
                if(isDigit(ch) == true) 
                number();
                else if(isAlpha(ch) == true)
                identifier();
                else
                Metal.error(line, "unexpected error or character"); 
                break;
            }
        }
    }
    
    public void string()
    {
        while(peek() != '"' && isAtEnd() == false)
        {
            if(peek() == '\n') line++;
            advance();
        }
        
        if(isAtEnd() == true)
        {
            Metal.error(line, "string not terminated");
            return;
        }
        advance();
        String ret = source.substring(start + 1, current - 1);
        addToken(TokenType.STRING, ret);
    }
    
    public boolean isDigit(char ch)
    {
        if(ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch =='9')
        return true;
        return false;
    }
    
    public void number()
    {
        while(isDigit(peek()) == true) advance();
        if(peek() == '.' && isDigit(peekNext()) == true)
        {
            advance();
            while(isDigit(peek()) == true)
            advance();
        }
        addToken(TokenType.NUMBER, Double.parseDouble(source.substring(start, current)));
    }
    
    public boolean isAlpha(char ch)
    {
        if((ch >= 'a' && ch <'z') || (ch >= 'A' && ch <='Z') || (ch == '_'))
        return true;
        return false;
    }
    
    public boolean isAlphaNumeric(char ch)
    {
        if(isAlpha(ch) == true || isDigit(ch) == true)
        return true;
        return false;
    }
    
    private void identifier()
    {
        while(isAlphaNumeric(peek()) == true) advance();
        String part = source.substring(start, current);
        TokenType type = keywords.get(part);
        if(type == null) type = TokenType.IDENTIFIER;
        addToken(type);
    }
    
    public char advance()
    {
        current++;
        return source.charAt(current - 1);
    }
    
    public boolean match(char expected)
    {
        if(isAtEnd() == true) return false;
        if(source.charAt(current) != expected) return false;
        current++;
        return true;
    }
    
    public char peek()
    {
        if(isAtEnd() == true) return '\0';
        return source.charAt(current);
    }
    
    public char peekNext()
    {
        if(current + 1 >= source.length()) return '\0';
        return source.charAt(current + 1);
    }
    
    public void addToken(TokenType type)
    {
        addToken(type, null);
    }
    
    public void addToken(TokenType type, Object literal)
    {
        String text = source.substring(start, current);
        tokens.add(new Token(type, text, literal, line));
    }
    
    public boolean isAtEnd()
    {
        if(current >= source.length())
        return true;
        return false;
    }
}