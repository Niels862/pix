#include "lexer.hpp"
#include <iostream>

int main() {
    Lexer lexer("test.pix");
    std::vector<Token> tokens = lexer.lex();
    
    for (Token const &token : tokens) {
        std::cout << token << std::endl;
    }

    return 0;
}
