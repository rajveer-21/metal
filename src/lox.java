import java.io.*;
import java.util.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.charset.Charset;

class Lox
{
    public static boolean hadError = false;
    public static void main(String args[])throws IOException
    {
        if(args.length >= 2)
        System.out.println("Metal(Error Code : 65)");
        if(args.length == 1)
        run_file(args[0]);
        if(args.length == 0)
        run_console();
    }
    public static void run_file(String path)throws IOException
    {
        byte bytes[] = Files.readAllBytes(Paths.get(path));
        run(new String(bytes, Charset.defaultCharset()));
        if(hadError == true)
        System.exit(65);
    }
    public static void run_console()throws IOException
    {
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        while(true)
        {
            String current_line = br.readLine();
            if(current_line == null)
            break;
            run(current_line);
            hadError = false;
        }
    }
    public static void run(String current)
    {
        Scanner scanner = new Scanner(current);
        List<Token> tokens = scanner.scanTokens();
        for(int i = 0; i < tokens.size(); i++)
        System.out.println(tokens.get(i));
    }
    public static void error(int line, String message)
    {
        report(line, message);
    }
    public static void report(int line, String message)
    {
        hadError = true;
        System.err.println(line + " had an error , namely - " + message);
    }
}
