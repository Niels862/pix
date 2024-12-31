#include "lexer.hpp"
#include "parser.hpp"
#include "json.hpp"
#include <iostream>

int main() {
    try {
        Lexer lexer("test.pix");

        std::vector<Token> tokens = lexer.lex();
        
        std::cout << "[" << std::endl;
        for (Token const &token : tokens) {
            std::cout << token << std::endl;
        }
        std::cout << "]" << std::endl;

        Parser parser(tokens);

        Node::ptr node = parser.parse();

        node->check_type();

        std::cout << *node->to_json() << std::endl;
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
