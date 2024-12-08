#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>


enum class TokenType {
    _return,
    int_lit,
    semi
};

// Helps to separate the source code to we transform it to binary
struct Token {
    TokenType type;
    std::optional<std::string> value;


    Token(TokenType _type){
        this->type = _type;
    }

    Token(TokenType _type, std::string _value){
        this->type = _type;
        this->value = _value;
    }
};


// Analyses the given string and returns its tokens as a vector or Tokens
std::vector<Token> tokenize(const std::string& str) {
    std::vector<Token> tokens;

    std::string buffer;
    for(int i=0; i < str.length(); i++) {
        char c = str.at(i);
        if(std::isalpha(c)){
            buffer.push_back(c);
            i++;
            while(std::isalnum(str.at(i))){
                buffer.push_back(str.at(i));
                i++;
            }
            i--;

            if(buffer == "return"){
                tokens.emplace_back(TokenType::_return);
                buffer.clear();
                continue;
            }else{
                std::cerr << "Something went wrong" << std::endl;
                exit(EXIT_FAILURE);
            }
        }else if(std::isdigit(c)){
            buffer.push_back(c);
            i++;
            while(std::isdigit(str.at(i))){
                buffer.push_back(str.at(i));
                i++;
            }
            i--;
            tokens.emplace_back(TokenType::int_lit, buffer);
            buffer.clear();
        }else if(c == ';'){
            tokens.emplace_back(TokenType::semi);
        }else if(std::isspace(c)){
            continue;
        }else{
            std::cerr << "Something went wrong" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    return tokens;
}


// Not normal to transform tokens directly to assembly but FOR NOW we have this, next time i create the parse tree
std::string tokensToAsm(const std::vector<Token>& tokens){
    std::stringstream strAsm;
    strAsm << "global _start\n_start:\n";
    for(int i=0; i < tokens.size(); i++){
        const Token& token = tokens.at(i);
        if(token.type == TokenType::_return){
            if(i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit){ // Checks if next token is a int literal
                if(i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi){ // Checks if the next next token is a semicolon
                    strAsm << "    mov rax, 60\n";
                    strAsm << "    mov rdi, " << tokens.at(i + 1).value.value() << "\n";
                    strAsm << "    syscall";
                }
            }
        }
    }

    return strAsm.str();
}

int main(int argc, char* argv[]){
    // argc = arguments counter
    // argv[] = array of strings
    if(argc != 2){
        std::cerr << "Incorrect usage, please use: " << std::endl;
        std::cerr << "electro <input.et>" << std::endl;
        return EXIT_FAILURE;
    }

    // Gets the source code content
    std::fstream input(argv[1], std::ios::in);          // defines that argv[1] will be treated as a file INput
    std::stringstream contentsStream;
    contentsStream << input.rdbuf();                   // sends the input buffer to contents string stream
    std::string contents = contentsStream.str();       // Contructs the final string
    input.close();

    // Generates the tokens vector
    std::vector<Token> tokens = tokenize(contents);

    // Generates the assembly file
    std::fstream file("out.asm", std::ios::out);
    file << tokensToAsm(tokens);
    file.close();

    // Generates the class file
    system("nasm -f elf64 out.asm");
    // Generates the executable file
    system("ld out.o -o out");

    return EXIT_SUCCESS;
}
