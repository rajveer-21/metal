#include <vector>
#include <string>
#include <memory>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include "scanner.hpp"
#include "interpreter.hpp"

bool hadError = false;
void run(const std::string& source)
{
    scanner scanner1(source);
    std::vector<std::shared_ptr<Token>> tokens = scanner1.scan_tokens();
    parser parser1(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser1.parse();
    if(hadError == true)
    return;
    interpreter interpreter1;
    interpreter1.interpret(statements);
}

void run_file(const std::string& source)
{
    std::ifstream file(source, std::ios::binary);
    if(!file)
    {
        std::cerr << "Unable to open file at given path : " << source << std::endl;
        std::exit(65);
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());
}

void run_prompt()
{
    std::string current;
    for(;;)
    {
        std::cout << "> ";
        std::cout.flush();
        std::getline(std::cin, current);
        if(current == "")
        break;
        run(current);
    }
}

std::shared_ptr<Token> make_token(TokenType type, const std::string& lexeme)
{
    return std::make_shared<Token>(lexeme, std::any{}, type, 1);
}

void metal_error(std::shared_ptr<Token> token, const std::string& message)
{
    if(token->type == TokenType::EOF_TOKEN)
    std::cerr << "Reached end of source program without completion of expression." << std::endl;
    std::cerr << message << " at line " << token->line << std::endl;
}

void metal_runtime_error(const RuntimeError& error)
{
    std::cerr << error.what() << " at line : " << error.token->line << std::endl;
    hadError = true;
    return;
}

int main(int argc, char* argv[])
{
    if(argc > 2)
    {
        std::cout << "Usage error, exiting." << std::endl;
        std::exit(64);
    }
    else if(argc == 2)
    {
        run_file(argv[1]);
    }
    else
    {
        run_prompt();
    }
}