import java.io.*;
import java.util.*;
import java.nio.file.*;
import java.nio.charset.*;

class Metal
{
    public static final Interpreter = new Interpreter();
    public static boolean hadError = false;
    public static boolean hadRuntimeError = false;
    public static void main(String args[])throws IOException
    {
        if(args.length > 1)
        {
            System.out.println("invalid input to the command line, provide a single file or use input terminal");
            System.exit(64);
        }
        if(args.length == 1)
        {
            runInpArg(args[0]);
        }
        else
        {
            runPrompt();
        }
    }
    
    public static void runInpArg(String path)throws IOException
    {
        byte bytes[] = Files.readAllBytes(Paths.get(path));
        String input = new String(bytes, Charset.defaultCharset());
        run(input);
        if(hadError == true) 
        System.exit(65);
        if(hadRuntimeError == true)
        System.exit(70);
    }
    
    public static void runPrompt()throws IOException
    {
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        while(true)
        {
            System.out.print(">");
            String line = br.readLine();
            if(line == null)
            break;
            run(line);
            hadError = false;
        }
    }
    
    public static void run(String source)
    {
        Scanner scanner = new Scanner(source);
        List<Token> tokens = scanner.scanTokens();
        Parser parser = new Parser(tokens);
        Expr expression = parser.parse();
        if(hadError == true)
        return;
        interpreter.interpret(expression);
    }
    
    public static void error(int line, String message)
    {
        reportError(line, "", message);
    }
    
    public static void error(Token token, String message)
    {
        if(token.type == TokenType.EOF)
        {
            reportError(token.line, " is at end ", message);
        }
        else
        {
            reportError(token.line, " at " + token.lexeme + "'", message);
        }
    }
    
    public static void reportError(int line, String where, String message)
    {
        System.err.println("Error at line - " + line + " at - " + where + " of type - " + message + " .");
        hadError = true;
    }
    
    public static void runtimeError(RuntimeError error)
    {
        System.err.println(error.getMessage() + " at line " + error.token.line );
        hadRuntimeError = true;
    }
}