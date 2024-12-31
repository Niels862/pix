#include "lexer.hpp"
#include <iostream>

int main() {
    Lexer lexer("test.pix");

    try {
        std::vector<Token> tokens = lexer.lex();
        
        for (Token const &token : tokens) {
            std::cout << token << std::endl;
        }
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
