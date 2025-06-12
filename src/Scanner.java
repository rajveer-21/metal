import java.util.*;
class Scanner
{
    private String source;
    private int start = 0;
    private int line = 1;
    private int current = 0;
    private final static List<Token> tokens = new ArrayList<>();
    private static final HashMap<String, TokenType> keywords;
    static
    {
        keywords = new HashMap<>();
        keywords.put("and", TokenType.AND);
        keywords.put("class", TokenType.CLASS);
        keywords.put("else", TokenType.ELSE);
        keywords.put("false", TokenType.FALSE);
        keywords.put("for", TokenType.FOR);
        keywords.put("fun", TokenType.FUN);
        keywords.put("if", TokenType.IF);
        keywords.put("nil", TokenType.NIL);
        keywords.put("or", TokenType.OR);
        keywords.put("print", TokenType.PRINT);
        keywords.put("return", TokenType.RETURN);
        keywords.put("super", TokenType.SUPER);
        keywords.put("this", TokenType.THIS);
        keywords.put("true", TokenType.TRUE);
        keywords.put("var", TokenType.VAR);
        keywords.put("while", TokenType.WHILE);
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
        tokens.add(new Token(line, "", TokenType.EOF, null));
        return tokens;
    }
    private boolean isAtEnd()
    {
        if(current >= source.length())
        return true;
        return false;
    }
    private void scanToken()
    {
        char ch = advance();
        switch(ch)
        {
            case '.' : addToken(TokenType.DOT); break;
            case '*' : addToken(TokenType.STAR); break;
            case '+' : addToken(TokenType.PLUS); break;
            case ',' : addToken(TokenType.COMMA); break;
            case '/' : addToken(TokenType.SLASH); break;
            case '-' : addToken(TokenType.MINUS); break;
            case ';' : addToken(TokenType.SEMICOLON); break;
            case '(' : addToken(TokenType.LEFT_PAREN); break;
            case '{' : addToken(TokenType.LEFT_BRACE); break;
            case ')' : addToken(TokenType.RIGHT_PAREN); break;
            case '}' : addToken(TokenType.RIGHT_BRACE); break;
            case '=' :
            {
                if(match('=') == true)
                {
                    addToken(TokenType.EQUAL_EQUAL);
                }
                else
                {
                    addToken(TokenType.EQUAL);
                }
                break;
            }
            case '!' :
            {
                if(match('=') == true)
                {
                    addToken(TokenType.BANG_EQUAL);
                }
                else
                {
                    addToken(TokenType.BANG);
                }
                break;
            }
            case '>' :
            {
                if(match('=') == true)
                {
                    addToken(TokenType.GREATER_EQUAL);
                }
                else
                {
                    addToken(TokenType.GREATER);
                }
                break;
            }
            case '<':
            {
                if(match('=') == true)
                {
                    addToken(TokenType.LESS_EQUAL);
                }
                else
                {
                    addToken(TokenType.LESS);
                }
                break;
            }
            case ' '  : break;
            case '\t' : break;
            case '\n' : line++; break; 
            default: 
            {
                if(isDigit(ch) == true)
                {
                    number();
                }
                else if(isAlpha(ch) == true)
                {
                    identifier();
                }
                else
                Lox.error(line, "Unexpected Error"); 
                break;
            }
        }
    }
    private char advance()
    {
        current++;
        return source.charAt(current - 1);
    }
    private void addToken(TokenType type) 
    {
        addToken(type, null);
    }
    private void addToken(TokenType type, Object literal) 
    {
        String text = source.substring(start, current);
        tokens.add(new Token(line, text, type, literal));
    }
    private boolean match(char next)
    {
        if(isAtEnd() == true) return false;
        if(source.charAt(current) != next)return false;
        current++;
        return true;
    }
    private char peek()
    {
        if(isAtEnd() == true)
        return '\0';
        else
        return source.charAt(current);
    }
    private char peekNext()
    {
        if(current + 1 >= source.length()) return '\0';
        else
        return source.charAt(current + 1);
    }
    private boolean isDigit(char ch)
    {
        if(ch >= '0' && ch <= '9')
        return true;
        return false;
    }
    private boolean isAlpha(char ch)
    {
        if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
        return true;
        return false;
    }
    private boolean isAlphaNumeric(char ch)
    {
        if(isDigit(ch) || isAlpha(ch))
        return true;
        return false;
    }
    private void number()
    {
        while(isDigit(peek()) == true)
        advance();
        if(peek() == '.' && isDigit(peekNext()))
        {
            advance();
            while(isDigit(peek()) == true)
            advance();
        }
        addToken(TokenType.NUMBER, Double.parseDouble(source.substring(start, current)));
    }
    private void identifier()
    {
        while(isAlphaNumeric(peek())) advance();
        String substring = source.substring(start, current);
        TokenType type = keywords.get(substring);
        if(type == null) type = TokenType.IDENTIFIER;
        addToken(type);
    }
}